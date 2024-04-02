#include "X86.h"
#include "X86InstrInfo.h"
#include "X86InstrBuilder.h"
#include "X86Subtarget.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
//#include "llvm/Target/TargetInstrInfo.h" no this file, why?
#include "llvm/MC/MCContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#define DEBUG_TYPE "minfootprint-backend"

using namespace llvm;

STATISTIC(NumRandStackFrame, "Number of random stack frame (instrumented)");
STATISTIC(NumRandStackNoEpil, "Number of random stack frame (failed, epilogue)");
STATISTIC(NumRandStackNoProl1, "Number of random stack frame (failed, prologue1)");
STATISTIC(NumRandStackNoProl2, "Number of random stack frame (failed, prologue2)");

namespace {

#define DOUT(msg) do {                          \
    if (true) {                    \
        errs() << "[X86] " << msg;                \
    }                                           \
} while (0)

    struct X86MinFootprint : public MachineFunctionPass {
    public:
        MachineFunction *MF;
        static char ID;
        X86MinFootprint() : MachineFunctionPass(ID) {}

        bool runOnMachineFunction(MachineFunction &MF) override;
    private:
        const TargetInstrInfo *TII;
        const TargetRegisterInfo *TRI;
        const TargetMachine *TM;
        MachineFrameInfo *FrameInfo;

        //lcy
        bool enforceRSPFramePreserving(MachineFunction &MF);

        bool isTargetFunctionHasAttributeOblivious(MachineInstr &MI);


};
char X86MinFootprint::ID = 0;
}


bool X86MinFootprint::enforceRSPFramePreserving(MachineFunction &Func)
{
    MachineInstr *Prologue = nullptr; // push rbp
    MachineInstr *SFP = nullptr; // mov rsp, rbp
    MachineInstr *SFPNext = nullptr; // Instrumentation point
    SmallVector<MachineInstr*, 8> EpilogueList;

    MachineBasicBlock::iterator BBI = Func.begin()->begin();
    Prologue = &*(BBI++);

    if (Prologue->getOpcode() != X86::PUSH64r
            || Prologue->getOperand(0).getReg() != X86::RBP) {
        DOUT("Wrong Prologue: " << *Prologue << "\n");
        NumRandStackNoProl1++;
        return false;
    }

    // Skip over Call Frame Instructions
    do {
        MachineInstr *I = &*BBI;
        if (I->getOpcode() != TargetOpcode::CFI_INSTRUCTION)
            break;
        BBI++;
    } while(1);

    SFP = &*(BBI++);

    if (SFP->getOpcode() != X86::MOV64rr
            || SFP->getOperand(0).getReg() != X86::RBP
            || SFP->getOperand(1).getReg() != X86::RSP) {
        DOUT("Wrong SFP:" << *SFP << "\n");
        SFP = nullptr;
        NumRandStackNoProl2++;
        return false;
    }

    //Again Skip over Call Frame Instructions
    do {
        MachineInstr *I = &*BBI;
        if (I->getOpcode() != TargetOpcode::CFI_INSTRUCTION)
            break;
        BBI++;
    } while(1);

    SFPNext = &*(BBI);
    //examine whether it is reserving stack frame

    //assume the reserving instruction is in first basic block
    MachineFunction::iterator MFI = Func.begin();
    for (MachineBasicBlock::iterator BBI2 = MFI->begin();BBI2 != MFI->end(); ++BBI2) {
        MachineInstr *MI = &*(BBI2);

        if( (MI->getOpcode() == X86::SUB64ri32 || MI->getOpcode() == X86::SUB64ri8)
                &&  MI->getOperand(0).isReg()
                &&  MI->getOperand(0).getReg() == X86::RSP)
        {
            DOUT("It is already reserving stack frame\n");
            return false;
        }
        if (MI->isReturn()||MI->isCall()) {
            //it should be before call, return
            break;
        }
    }

    
        DOUT("-----------------SFPNext dump!-----------------------\n");
    if (true)
    {
        SFPNext->dump();
    }
    DOUT("-----------------dump end---------------------------\n");
    

    // Collect all epilogues

    for (MachineFunction::iterator MFI = Func.begin(), MFE = Func.end();
            MFI != MFE; ++MFI) {
        for (MachineBasicBlock::iterator BBI = MFI->begin();
                BBI != MFI->end(); ++BBI) {
            MachineInstr *MI = &*BBI;

            if (MI->isReturn()) {
                MachineInstr *PopRBP = &*std::prev(BBI);
                if (PopRBP->getOpcode() == X86::POP64r
                        && PopRBP->getOperand(0).getReg() == X86::RBP) {
                    // MachineInstr *PrevPopRBP = &*std::prev(std::prev(BBI));
                    if (PopRBP != nullptr)
                        EpilogueList.push_back(PopRBP);
                }
            }
        }
    }

    if (EpilogueList.size() <= 0) {
        // Cannot find proper prologue/epilogues, so simply punt these cases.
        DOUT("No Epilogue\n");
        NumRandStackNoEpil++;
        return false;
    }

    // Instrument prologue
    MachineBasicBlock &PBB = *SFPNext->getParent();
    DebugLoc DL = SFPNext->getDebugLoc();
    BuildMI(PBB, SFPNext, DL, TII->get(X86::SUB64ri32))
        .addReg(X86::RSP, RegState::Define)
        .addReg(X86::RSP)
        .addImm(FrameInfo->estimateStackSize(*MF));



    // Instrument epilogue
    for(MachineInstr *PopRBP: EpilogueList) {
        MachineBasicBlock &EBB = *PopRBP->getParent();

        BuildMI(EBB, PopRBP, PopRBP->getDebugLoc(), TII->get(X86::ADD64ri32))
            .addReg(X86::RSP, RegState::Define)
            .addReg(X86::RSP)
            .addImm(FrameInfo->estimateStackSize(*MF));

    }


    NumRandStackFrame++;
    return true;
}
//static RegisterPass<MachineCountPass> X("machinecount","Machine Count Pass");

bool X86MinFootprint::runOnMachineFunction(MachineFunction &Func) {
    bool modified = false;
    //MF = &Func;
    
    MF = &Func;
    TM = &Func.getTarget();
    TII = Func.getSubtarget().getInstrInfo();
    TRI = Func.getSubtarget().getRegisterInfo();
    FrameInfo = &Func.getFrameInfo();

    DOUT("Function Name :" << MF->getName().str().c_str() << ","<< FrameInfo->getNumObjects()<<" objects\n");

    modified |= enforceRSPFramePreserving(Func);
    return modified;
}

FunctionPass *llvm::createX86MinFootprintPass() {
    return new X86MinFootprint();
}

