#include "IRCompiler.h"
#include <circle/logger.h>

void IRCompiler::RunOptimiser(std::list<IRInstruction>* _ir)
{
	IRInstruction* prev1 = nullptr;
	IRInstruction* prev2 = nullptr;
	IRInstruction* prev3 = nullptr;
	int count = 0;
	do {
		count = 0;
		for (auto& cur: *_ir) {
			if (prev2==nullptr && prev1==nullptr) {
				// First run, no previous, 1 opcode
			}
			else if (prev1!=nullptr) {
				if (prev1->type==IROpcodes::StackPushFloatOperand1 && cur.type==IROpcodes::StackPopFloatOperand1) {
					prev1->type = IROpcodes::NOP;
					cur.type = IROpcodes::NOP;
					count++;
				}
				else if (prev1->type==IROpcodes::StackPushFloatOperand2 && cur.type==IROpcodes::StackPopFloatOperand2) {
					prev1->type = IROpcodes::NOP;
					cur.type = IROpcodes::NOP;
					count++;
				}
				else if (prev1->type==IROpcodes::StackPushIntOperand1 && cur.type==IROpcodes::StackPopIntOperand1) {
					prev1->type = IROpcodes::NOP;
					cur.type = IROpcodes::NOP;
					count++;
				}
				else if (prev1->type==IROpcodes::StackPushIntOperand2 && cur.type==IROpcodes::StackPopIntOperand2) {
					prev1->type = IROpcodes::NOP;
					cur.type = IROpcodes::NOP;
					count++;
				}
				else if (prev1->type==IROpcodes::StackPushFloatOperand1 && cur.type==IROpcodes::StackPopFloatOperand2) {
					prev1->type = IROpcodes::NOP;
					cur.type = IROpcodes::FloatOperand1To2;
					count++;
				}
				else if (prev1->type==IROpcodes::StackPushFloatOperand2 && cur.type==IROpcodes::StackPopFloatOperand1) {
					prev1->type = IROpcodes::NOP;
					cur.type = IROpcodes::FloatOperand2To1;
					count++;
				}
				else if (prev1->type==IROpcodes::StackPushIntOperand1 && cur.type==IROpcodes::StackPopIntOperand2) {
					prev1->type = IROpcodes::NOP;
					cur.type = IROpcodes::IntegerOperand1To2;
					count++;
				}
				else if (prev1->type==IROpcodes::StackPushIntOperand2 && cur.type==IROpcodes::StackPopIntOperand1) {
					prev1->type = IROpcodes::NOP;
					cur.type = IROpcodes::IntegerOperand2To1;
					count++;
				}

				if (prev2!=nullptr) {
					if (prev3!=nullptr) {
						// Consolidate PUSHes then POPs
						if (prev3->type==IROpcodes::StackPushFloatOperand2 && prev2->type==IROpcodes::StackPushFloatOperand1
								&& prev1->type==IROpcodes::StackPopFloatOperand1 && cur.type==IROpcodes::StackPopFloatOperand2) {
							prev3->type = IROpcodes::NOP;
							prev2->type = IROpcodes::NOP;
							prev1->type = IROpcodes::NOP;
							cur.type = IROpcodes::NOP;
							count++;
						}
						else if (prev3->type==IROpcodes::StackPushFloatOperand1 && prev2->type==IROpcodes::StackPushFloatOperand2
								&& prev1->type==IROpcodes::StackPopFloatOperand1 && cur.type==IROpcodes::StackPopFloatOperand2) {
							prev3->type = IROpcodes::NOP;
							prev2->type = IROpcodes::NOP;
							prev1->type = IROpcodes::NOP;
							cur.type = IROpcodes::SwapFloatOperands;
							count++;
						}
						else if (prev3->type==IROpcodes::StackPushFloatOperand2 && prev2->type==IROpcodes::StackPushFloatOperand1
								&& prev1->type==IROpcodes::StackPopFloatOperand2 && cur.type==IROpcodes::StackPopFloatOperand1) {
							prev3->type = IROpcodes::NOP;
							prev2->type = IROpcodes::NOP;
							prev1->type = IROpcodes::NOP;
							cur.type = IROpcodes::SwapFloatOperands;
							count++;
						}
						else if (prev3->type==IROpcodes::StackPushIntOperand1 && prev2->type==IROpcodes::StackPushIntOperand2
								&& prev1->type==IROpcodes::StackPopIntOperand1 && cur.type==IROpcodes::StackPopIntOperand2) {
							prev3->type = IROpcodes::NOP;
							prev2->type = IROpcodes::NOP;
							prev1->type = IROpcodes::NOP;
							cur.type = IROpcodes::SwapIntegerOperands;
							count++;
						}
						else if (prev3->type==IROpcodes::StackPushIntOperand2 && prev2->type==IROpcodes::StackPushIntOperand1
								&& prev1->type==IROpcodes::StackPopIntOperand2 && cur.type==IROpcodes::StackPopIntOperand1) {
							prev3->type = IROpcodes::NOP;
							prev2->type = IROpcodes::NOP;
							prev1->type = IROpcodes::NOP;
							cur.type = IROpcodes::SwapIntegerOperands;
							count++;
						}
						else if (prev3->type==IROpcodes::VariableLoadInteger && prev2->type==IROpcodes::VariableLoadInteger
								&& prev1->type==IROpcodes::StackPopIntOperand1 && cur.type==IROpcodes::StackPopIntOperand2) {
							prev3->type = IROpcodes::NOP;
							prev2->type = IROpcodes::NOP;
							prev1->type = IROpcodes::NOP;
							cur.type = IROpcodes::LoadIntegerOperands;
							cur.index = prev3->index;
							cur.index2 = prev2->index;
							count++;
						}
						else if (prev3->type==IROpcodes::VariableLoadFloat && prev2->type==IROpcodes::VariableLoadFloat
								&& prev1->type==IROpcodes::StackPopFloatOperand1 && cur.type==IROpcodes::StackPopFloatOperand2) {
							prev3->type = IROpcodes::NOP;
							prev2->type = IROpcodes::NOP;
							prev1->type = IROpcodes::NOP;
							cur.type = IROpcodes::LoadFloatOperands;
							cur.index = prev3->index;
							cur.index2 = prev2->index;
							count++;
						}
					}
				}
			}

			// Shift along
			prev3 = prev2;
			prev2 = prev1;
			prev1 = &cur;
		}

		// Delete NOP tokens
		auto previous = _ir->size();
		_ir->remove_if([](IRInstruction ir) { return ir.type==IROpcodes::NOP; });

		// Summary
#ifdef VERBOSE_COMPILE
		CLogger::Get()->Write("IR Optimiser", LogDebug, "%d changes. %d micro-ops before, %d micro-ops after", count, previous, _ir->size());
#endif
	}
	while (count>0);
}
