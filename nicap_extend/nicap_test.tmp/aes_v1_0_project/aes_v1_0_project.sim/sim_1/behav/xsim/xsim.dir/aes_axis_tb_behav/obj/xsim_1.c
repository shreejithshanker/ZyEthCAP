/**********************************************************************/
/*   ____  ____                                                       */
/*  /   /\/   /                                                       */
/* /___/  \  /                                                        */
/* \   \   \/                                                         */
/*  \   \        Copyright (c) 2003-2013 Xilinx, Inc.                 */
/*  /   /        All Right Reserved.                                  */
/* /---/   /\                                                         */
/* \   \  /  \                                                        */
/*  \___\/\___\                                                       */
/**********************************************************************/


#include "iki.h"
#include <string.h>
#include <math.h>
#ifdef __GNUC__
#include <stdlib.h>
#else
#include <malloc.h>
#define alloca _alloca
#endif
/**********************************************************************/
/*   ____  ____                                                       */
/*  /   /\/   /                                                       */
/* /___/  \  /                                                        */
/* \   \   \/                                                         */
/*  \   \        Copyright (c) 2003-2013 Xilinx, Inc.                 */
/*  /   /        All Right Reserved.                                  */
/* /---/   /\                                                         */
/* \   \  /  \                                                        */
/*  \___\/\___\                                                       */
/**********************************************************************/


#include "iki.h"
#include <string.h>
#include <math.h>
#ifdef __GNUC__
#include <stdlib.h>
#else
#include <malloc.h>
#define alloca _alloca
#endif
typedef void (*funcp)(char *, char *);
extern void execute_213(char*, char *);
extern void execute_214(char*, char *);
extern void execute_215(char*, char *);
extern void execute_371(char*, char *);
extern void execute_372(char*, char *);
extern void execute_373(char*, char *);
extern void execute_374(char*, char *);
extern void execute_375(char*, char *);
extern void execute_376(char*, char *);
extern void execute_377(char*, char *);
extern void execute_378(char*, char *);
extern void execute_379(char*, char *);
extern void execute_380(char*, char *);
extern void execute_381(char*, char *);
extern void execute_382(char*, char *);
extern void execute_383(char*, char *);
extern void execute_384(char*, char *);
extern void execute_385(char*, char *);
extern void execute_386(char*, char *);
extern void execute_387(char*, char *);
extern void execute_388(char*, char *);
extern void execute_209(char*, char *);
extern void execute_210(char*, char *);
extern void execute_220(char*, char *);
extern void execute_221(char*, char *);
extern void vlog_simple_process_execute_0_fast_no_reg_no_agg(char*, char*, char*);
extern void execute_370(char*, char *);
extern void execute_4(char*, char *);
extern void execute_5(char*, char *);
extern void execute_6(char*, char *);
extern void execute_7(char*, char *);
extern void execute_8(char*, char *);
extern void execute_9(char*, char *);
extern void execute_10(char*, char *);
extern void execute_11(char*, char *);
extern void execute_12(char*, char *);
extern void execute_230(char*, char *);
extern void execute_231(char*, char *);
extern void vlog_const_rhs_process_execute_0_fast_no_reg_no_agg(char*, char*, char*);
extern void execute_292(char*, char *);
extern void execute_293(char*, char *);
extern void execute_17(char*, char *);
extern void execute_19(char*, char *);
extern void execute_238(char*, char *);
extern void execute_239(char*, char *);
extern void vlog_simple_process_execute_1_fast_no_reg_no_agg(char*, char*, char*);
extern void execute_289(char*, char *);
extern void execute_290(char*, char *);
extern void execute_21(char*, char *);
extern void execute_252(char*, char *);
extern void execute_240(char*, char *);
extern void execute_244(char*, char *);
extern void execute_245(char*, char *);
extern void execute_246(char*, char *);
extern void execute_248(char*, char *);
extern void execute_249(char*, char *);
extern void execute_250(char*, char *);
extern void execute_26(char*, char *);
extern void execute_27(char*, char *);
extern void execute_28(char*, char *);
extern void execute_29(char*, char *);
extern void execute_30(char*, char *);
extern void execute_31(char*, char *);
extern void execute_32(char*, char *);
extern void execute_33(char*, char *);
extern void execute_34(char*, char *);
extern void execute_35(char*, char *);
extern void execute_253(char*, char *);
extern void execute_254(char*, char *);
extern void execute_259(char*, char *);
extern void execute_260(char*, char *);
extern void execute_266(char*, char *);
extern void execute_37(char*, char *);
extern void execute_288(char*, char *);
extern void execute_276(char*, char *);
extern void execute_280(char*, char *);
extern void execute_281(char*, char *);
extern void execute_282(char*, char *);
extern void execute_284(char*, char *);
extern void execute_285(char*, char *);
extern void execute_286(char*, char *);
extern void execute_363(char*, char *);
extern void execute_364(char*, char *);
extern void execute_45(char*, char *);
extern void execute_47(char*, char *);
extern void execute_300(char*, char *);
extern void execute_301(char*, char *);
extern void execute_360(char*, char *);
extern void execute_361(char*, char *);
extern void execute_54(char*, char *);
extern void execute_55(char*, char *);
extern void execute_56(char*, char *);
extern void execute_57(char*, char *);
extern void execute_58(char*, char *);
extern void execute_59(char*, char *);
extern void execute_60(char*, char *);
extern void execute_61(char*, char *);
extern void execute_62(char*, char *);
extern void execute_324(char*, char *);
extern void execute_325(char*, char *);
extern void execute_326(char*, char *);
extern void execute_327(char*, char *);
extern void execute_328(char*, char *);
extern void execute_329(char*, char *);
extern void execute_330(char*, char *);
extern void execute_332(char*, char *);
extern void execute_333(char*, char *);
extern void execute_334(char*, char *);
extern void execute_335(char*, char *);
extern void execute_336(char*, char *);
extern void execute_337(char*, char *);
extern void execute_338(char*, char *);
extern void execute_339(char*, char *);
extern void execute_340(char*, char *);
extern void execute_341(char*, char *);
extern void execute_342(char*, char *);
extern void execute_343(char*, char *);
extern void execute_344(char*, char *);
extern void execute_346(char*, char *);
extern void execute_93(char*, char *);
extern void execute_95(char*, char *);
extern void execute_96(char*, char *);
extern void execute_97(char*, char *);
extern void execute_98(char*, char *);
extern void execute_150(char*, char *);
extern void execute_101(char*, char *);
extern void execute_102(char*, char *);
extern void execute_118(char*, char *);
extern void execute_119(char*, char *);
extern void execute_120(char*, char *);
extern void execute_121(char*, char *);
extern void execute_122(char*, char *);
extern void execute_125(char*, char *);
extern void execute_124(char*, char *);
extern void execute_146(char*, char *);
extern void execute_147(char*, char *);
extern void execute_148(char*, char *);
extern void execute_149(char*, char *);
extern void execute_175(char*, char *);
extern void execute_153(char*, char *);
extern void execute_154(char*, char *);
extern void execute_159(char*, char *);
extern void execute_160(char*, char *);
extern void execute_161(char*, char *);
extern void execute_162(char*, char *);
extern void execute_163(char*, char *);
extern void execute_164(char*, char *);
extern void execute_165(char*, char *);
extern void execute_166(char*, char *);
extern void execute_169(char*, char *);
extern void execute_171(char*, char *);
extern void execute_172(char*, char *);
extern void execute_173(char*, char *);
extern void execute_174(char*, char *);
extern void execute_177(char*, char *);
extern void execute_182(char*, char *);
extern void execute_183(char*, char *);
extern void execute_184(char*, char *);
extern void execute_185(char*, char *);
extern void execute_186(char*, char *);
extern void execute_187(char*, char *);
extern void execute_188(char*, char *);
extern void execute_189(char*, char *);
extern void execute_190(char*, char *);
extern void execute_191(char*, char *);
extern void execute_192(char*, char *);
extern void execute_193(char*, char *);
extern void execute_194(char*, char *);
extern void execute_201(char*, char *);
extern void execute_202(char*, char *);
extern void execute_204(char*, char *);
extern void execute_205(char*, char *);
extern void execute_207(char*, char *);
extern void execute_208(char*, char *);
extern void execute_217(char*, char *);
extern void execute_218(char*, char *);
extern void execute_219(char*, char *);
extern void execute_389(char*, char *);
extern void execute_390(char*, char *);
extern void execute_391(char*, char *);
extern void execute_392(char*, char *);
extern void execute_393(char*, char *);
extern void vlog_transfunc_eventcallback(char*, char*, unsigned, unsigned, unsigned, char *);
extern void transaction_21(char*, char*, unsigned, unsigned, unsigned);
extern void transaction_34(char*, char*, unsigned, unsigned, unsigned);
extern void transaction_35(char*, char*, unsigned, unsigned, unsigned);
extern void transaction_55(char*, char*, unsigned, unsigned, unsigned);
extern void transaction_57(char*, char*, unsigned, unsigned, unsigned);
extern void transaction_60(char*, char*, unsigned, unsigned, unsigned);
extern void transaction_63(char*, char*, unsigned, unsigned, unsigned);
extern void transaction_273(char*, char*, unsigned, unsigned, unsigned);
extern void vhdl_transfunc_eventcallback(char*, char*, unsigned, unsigned, unsigned, char *);
funcp funcTab[192] = {(funcp)execute_213, (funcp)execute_214, (funcp)execute_215, (funcp)execute_371, (funcp)execute_372, (funcp)execute_373, (funcp)execute_374, (funcp)execute_375, (funcp)execute_376, (funcp)execute_377, (funcp)execute_378, (funcp)execute_379, (funcp)execute_380, (funcp)execute_381, (funcp)execute_382, (funcp)execute_383, (funcp)execute_384, (funcp)execute_385, (funcp)execute_386, (funcp)execute_387, (funcp)execute_388, (funcp)execute_209, (funcp)execute_210, (funcp)execute_220, (funcp)execute_221, (funcp)vlog_simple_process_execute_0_fast_no_reg_no_agg, (funcp)execute_370, (funcp)execute_4, (funcp)execute_5, (funcp)execute_6, (funcp)execute_7, (funcp)execute_8, (funcp)execute_9, (funcp)execute_10, (funcp)execute_11, (funcp)execute_12, (funcp)execute_230, (funcp)execute_231, (funcp)vlog_const_rhs_process_execute_0_fast_no_reg_no_agg, (funcp)execute_292, (funcp)execute_293, (funcp)execute_17, (funcp)execute_19, (funcp)execute_238, (funcp)execute_239, (funcp)vlog_simple_process_execute_1_fast_no_reg_no_agg, (funcp)execute_289, (funcp)execute_290, (funcp)execute_21, (funcp)execute_252, (funcp)execute_240, (funcp)execute_244, (funcp)execute_245, (funcp)execute_246, (funcp)execute_248, (funcp)execute_249, (funcp)execute_250, (funcp)execute_26, (funcp)execute_27, (funcp)execute_28, (funcp)execute_29, (funcp)execute_30, (funcp)execute_31, (funcp)execute_32, (funcp)execute_33, (funcp)execute_34, (funcp)execute_35, (funcp)execute_253, (funcp)execute_254, (funcp)execute_259, (funcp)execute_260, (funcp)execute_266, (funcp)execute_37, (funcp)execute_288, (funcp)execute_276, (funcp)execute_280, (funcp)execute_281, (funcp)execute_282, (funcp)execute_284, (funcp)execute_285, (funcp)execute_286, (funcp)execute_363, (funcp)execute_364, (funcp)execute_45, (funcp)execute_47, (funcp)execute_300, (funcp)execute_301, (funcp)execute_360, (funcp)execute_361, (funcp)execute_54, (funcp)execute_55, (funcp)execute_56, (funcp)execute_57, (funcp)execute_58, (funcp)execute_59, (funcp)execute_60, (funcp)execute_61, (funcp)execute_62, (funcp)execute_324, (funcp)execute_325, (funcp)execute_326, (funcp)execute_327, (funcp)execute_328, (funcp)execute_329, (funcp)execute_330, (funcp)execute_332, (funcp)execute_333, (funcp)execute_334, (funcp)execute_335, (funcp)execute_336, (funcp)execute_337, (funcp)execute_338, (funcp)execute_339, (funcp)execute_340, (funcp)execute_341, (funcp)execute_342, (funcp)execute_343, (funcp)execute_344, (funcp)execute_346, (funcp)execute_93, (funcp)execute_95, (funcp)execute_96, (funcp)execute_97, (funcp)execute_98, (funcp)execute_150, (funcp)execute_101, (funcp)execute_102, (funcp)execute_118, (funcp)execute_119, (funcp)execute_120, (funcp)execute_121, (funcp)execute_122, (funcp)execute_125, (funcp)execute_124, (funcp)execute_146, (funcp)execute_147, (funcp)execute_148, (funcp)execute_149, (funcp)execute_175, (funcp)execute_153, (funcp)execute_154, (funcp)execute_159, (funcp)execute_160, (funcp)execute_161, (funcp)execute_162, (funcp)execute_163, (funcp)execute_164, (funcp)execute_165, (funcp)execute_166, (funcp)execute_169, (funcp)execute_171, (funcp)execute_172, (funcp)execute_173, (funcp)execute_174, (funcp)execute_177, (funcp)execute_182, (funcp)execute_183, (funcp)execute_184, (funcp)execute_185, (funcp)execute_186, (funcp)execute_187, (funcp)execute_188, (funcp)execute_189, (funcp)execute_190, (funcp)execute_191, (funcp)execute_192, (funcp)execute_193, (funcp)execute_194, (funcp)execute_201, (funcp)execute_202, (funcp)execute_204, (funcp)execute_205, (funcp)execute_207, (funcp)execute_208, (funcp)execute_217, (funcp)execute_218, (funcp)execute_219, (funcp)execute_389, (funcp)execute_390, (funcp)execute_391, (funcp)execute_392, (funcp)execute_393, (funcp)vlog_transfunc_eventcallback, (funcp)transaction_21, (funcp)transaction_34, (funcp)transaction_35, (funcp)transaction_55, (funcp)transaction_57, (funcp)transaction_60, (funcp)transaction_63, (funcp)transaction_273, (funcp)vhdl_transfunc_eventcallback};
const int NumRelocateId= 192;

void relocate(char *dp)
{
	iki_relocate(dp, "xsim.dir/aes_axis_tb_behav/xsim.reloc",  (void **)funcTab, 192);
	iki_vhdl_file_variable_register(dp + 81112);
	iki_vhdl_file_variable_register(dp + 81168);


	/*Populate the transaction function pointer field in the whole net structure */
}

void sensitize(char *dp)
{
	iki_sensitize(dp, "xsim.dir/aes_axis_tb_behav/xsim.reloc");
}

void simulate(char *dp)
{
	iki_schedule_processes_at_time_zero(dp, "xsim.dir/aes_axis_tb_behav/xsim.reloc");
	// Initialize Verilog nets in mixed simulation, for the cases when the value at time 0 should be propagated from the mixed language Vhdl net

	iki_vlog_schedule_transaction_signal_fast_vhdl_value_time_0(dp + 166680, dp + 84176, 0, 0, 0, 0, 1, 1);

	iki_vlog_schedule_transaction_signal_fast_vhdl_value_time_0(dp + 166544, dp + 93448, 0, 127, 0, 127, 128, 1);
	iki_execute_processes();

	// Schedule resolution functions for the multiply driven Verilog nets that have strength
	// Schedule transaction functions for the singly driven Verilog nets that have strength

}
#include "iki_bridge.h"
void relocate(char *);

void sensitize(char *);

void simulate(char *);

int main(int argc, char **argv)
{
    iki_heap_initialize("ms", "isimmm", 0, 2147483648) ;
    iki_set_sv_type_file_path_name("xsim.dir/aes_axis_tb_behav/xsim.svtype");
    iki_set_crvs_dump_file_path_name("xsim.dir/aes_axis_tb_behav/xsim.crvsdump");
    void* design_handle = iki_create_design("xsim.dir/aes_axis_tb_behav/xsim.mem", (void *)relocate, (void *)sensitize, (void *)simulate, 0, isimBridge_getWdbWriter(), 0, argc, argv);
     iki_set_rc_trial_count(100);
    (void) design_handle;
    return iki_simulate_design();
}
