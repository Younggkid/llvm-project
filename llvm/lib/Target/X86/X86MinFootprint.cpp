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

namespace {
    class MachineCountPass : public MachineFunctionPass {
    public:
        static char ID;
        MachineCountPass() : MachineFunctionPass(ID) {}

        virtual bool runOnMachineFunction(MachineFunction &MF) {
            unsigned num_instr = 0;
            for (MachineFunction::const_iterator I = MF.begin(), E = MF.end(); I != E; ++I) {
                for (MachineBasicBlock::const_iterator BBI = I-> begin(), BBE = I-> end(); BBI != BBE; ++BBI) {
                    ++num_instr;
                }
            }
            errs() << "mcount --- " << MF.getName() << "has" << num_instr << " instructions.\n";
            return false;
        }


    };
}

FunctionPass *llvm::createX86MinFootprintPass() {
    return new MachineCountPass();
}

char MachineCountPass::ID = 0;
//static RegisterPass<MachineCountPass> X("machinecount","Machine Count Pass");