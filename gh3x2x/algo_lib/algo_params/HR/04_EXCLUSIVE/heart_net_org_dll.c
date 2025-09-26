#if (_WIN32 || _WIN64) && !defined(Goodix_DSP_EXPORTS)
#include "heart_net_org_export.h"
void heart_net_dll_param_init()
{
	goodix_win_params st_dll_params_in;

	st_dll_params_in.knBasicWeights.weight_arr = get_knBasicWeightsArr_addr();
	st_dll_params_in.knBasicWeights.para_size = get_knBasicWeightsArr_size();
	get_knBasicWeightsArr_version(st_dll_params_in.knBasicWeights.net_version, VERSION_SIZE_MAX);

	st_dll_params_in.knWeightsSmall.weight_arr = get_knSmallWeightsArr_addr();
	st_dll_params_in.knWeightsSmall.para_size = get_knSmallWeightsArr_size();
	get_knSmallWeightsArr_version(st_dll_params_in.knWeightsSmall.net_version, VERSION_SIZE_MAX);

	st_dll_params_in.knWeights.weight_arr = get_knWeightsArr_addr();
	st_dll_params_in.knWeights.para_size = get_knWeightsArr_size();
	get_knWeightsArr_version(st_dll_params_in.knWeights.net_version, VERSION_SIZE_MAX);

	st_dll_params_in.knConfNetWeights.weight_arr = get_knConfNetWeightsArr_addr();
	st_dll_params_in.knConfNetWeights.para_size = get_knConfNetWeightsArr_size();
	get_knConfNetWeightsArr_version(st_dll_params_in.knConfNetWeights.net_version, VERSION_SIZE_MAX);

	st_dll_params_in.knSceneNetWeights.weight_arr = get_knSceneNetWeightsArr_addr();
	st_dll_params_in.knSceneNetWeights.para_size = get_knSceneNetWeightsArr_size();
	get_knSceneNetWeightsArr_version(st_dll_params_in.knSceneNetWeights.net_version, VERSION_SIZE_MAX);

	st_dll_params_in.knSceneSwitchWeights.weight_arr = get_knSceneSwitchWeightsArr_addr();
	st_dll_params_in.knSceneSwitchWeights.para_size = get_knSceneSwitchWeightsArr_size();
	get_knSceneSwitchWeightsArr_version(st_dll_params_in.knSceneSwitchWeights.net_version, VERSION_SIZE_MAX);

	st_dll_params_in.knMultiWeights.weight_arr = get_knMultiWeightsArr_addr();
	st_dll_params_in.knMultiWeights.para_size = get_knMultiWeightsArr_size();
	get_knMultiWeightsArr_version(st_dll_params_in.knMultiWeights.net_version, VERSION_SIZE_MAX);

	st_dll_params_in.knTdfusionWeights.weight_arr = get_knTdfusionWeightsArr_addr();
	st_dll_params_in.knTdfusionWeights.para_size = get_knTdfusionWeightsArr_size();
	get_knTdfusionWeightsArr_version(st_dll_params_in.knTdfusionWeights.net_version, VERSION_SIZE_MAX);

	goodix_heart_net_dll_set_params_inner(&st_dll_params_in);
}
#endif //#if (_WIN32 || _WIN64) 
