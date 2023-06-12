file(GLOB srcs "opus/src/*.c")
#file(GLOB silk "opus/silk/*.c")
#file(GLOB celt "opus/celt/*.c")
#file(GLOB fixed "opus/slik/fixed/*.c")
#file(GLOB float "opus/slik/float/*.c")
set(fixed "opus/silk/fixed/encode_frame_FIX.c" 
    "opus/silk/fixed/apply_sine_window_FIX.c"
    "opus/silk/fixed/k2a_FIX.c"
    "opus/silk/fixed/residual_energy16_FIX.c"
    "opus/silk/fixed/k2a_Q16_FIX.c"
    "opus/silk/fixed/residual_energy_FIX.c"
    "opus/silk/fixed/autocorr_FIX.c"
    "opus/silk/fixed/LTP_analysis_filter_FIX.c"
    "opus/silk/fixed/schur64_FIX.c"
    "opus/silk/fixed/burg_modified_FIX.c"
    "opus/silk/fixed/LTP_scale_ctrl_FIX.c"
    "opus/silk/fixed/schur_FIX.c"
    "opus/silk/fixed/corrMatrix_FIX.c"
    "opus/silk/fixed/vector_ops_FIX.c"
    "opus/silk/fixed/find_LPC_FIX.c"
    "opus/silk/fixed/noise_shape_analysis_FIX.c"
    "opus/silk/fixed/warped_autocorrelation_FIX.c"
    "opus/silk/fixed/find_LTP_FIX.c"
    "opus/silk/fixed/pitch_analysis_core_FIX.c"
    "opus/silk/fixed/find_pitch_lags_FIX.c"
    "opus/silk/fixed/process_gains_FIX.c"
    "opus/silk/fixed/find_pred_coefs_FIX.c"
    "opus/silk/fixed/regularize_correlations_FIX.c")
 
set (celt "opus/celt/entcode.c"
    "opus/celt/laplace.c"
    "opus/celt/quant_bands.c"
    "opus/celt/bands.c"
    "opus/celt/entdec.c"
    "opus/celt/mathops.c"
    "opus/celt/rate.c"
    "opus/celt/celt.c"
    "opus/celt/celt_decoder.c"
    "opus/celt/entenc.c"
    "opus/celt/mdct.c"
    "opus/celt/celt_encoder.c"
    "opus/celt/celt_lpc.c"
    "opus/celt/modes.c"
    "opus/celt/cwrs.c"
    "opus/celt/opus_custom_demo.c"
    "opus/celt/vq.c"
    "opus/celt/kiss_fft.c"
    "opus/celt/pitch.c")
set (silk 
    opus/silk/A2NLSF.c 
    opus/silk/resampler_private_down_FIR.c
    opus/silk/ana_filt_bank_1.c
    opus/silk/inner_prod_aligned.c
    opus/silk/interpolate.c
    opus/silk/resampler_private_IIR_FIR.c
    opus/silk/lin2log.c
    opus/silk/resampler_private_up2_HQ.c
    opus/silk/biquad_alt.c
    opus/silk/log2lin.c
    opus/silk/resampler_rom.c
    opus/silk/bwexpander_32.c
    opus/silk/LPC_analysis_filter.c
    opus/silk/resampler_rom.h
    opus/silk/bwexpander.c
    opus/silk/LPC_fit.c
    opus/silk/resampler_structs.h
    opus/silk/check_control_input.c
    opus/silk/LPC_inv_pred_gain.c
    opus/silk/shell_coder.c
    opus/silk/CNG.c
    opus/silk/LP_variable_cutoff.c
    opus/silk/sigm_Q15.c
    opus/silk/code_signs.c
    opus/silk/control_audio_bandwidth.c
    opus/silk/sort.c
    opus/silk/control_codec.c
    opus/silk/stereo_decode_pred.c
    opus/silk/stereo_encode_pred.c
    opus/silk/control_SNR.c
    opus/silk/stereo_find_predictor.c
    opus/silk/debug.c
    opus/silk/NLSF2A.c
    opus/silk/stereo_LR_to_MS.c
    opus/silk/NLSF_decode.c
    opus/silk/stereo_MS_to_LR.c
    opus/silk/dec_API.c
    opus/silk/NLSF_del_dec_quant.c
    opus/silk/stereo_quant_pred.c
    opus/silk/decode_core.c
    opus/silk/NLSF_encode.c
    opus/silk/decode_frame.c
    opus/silk/NLSF_stabilize.c
    opus/silk/sum_sqr_shift.c
    opus/silk/decode_indices.c
    opus/silk/NLSF_unpack.c
    opus/silk/table_LSF_cos.c
    opus/silk/decode_parameters.c
    opus/silk/NLSF_VQ.c
    opus/silk/tables_gain.c
    opus/silk/decode_pitch.c
    opus/silk/NLSF_VQ_weights_laroia.c
    opus/silk/decode_pulses.c
    opus/silk/NSQ.c
    opus/silk/tables_LTP.c
    opus/silk/decoder_set_fs.c
    opus/silk/NSQ_del_dec.c
    opus/silk/tables_NLSF_CB_NB_MB.c
    opus/silk/tables_NLSF_CB_WB.c
    opus/silk/enc_API.c
    opus/silk/tables_other.c
    opus/silk/encode_indices.c
    opus/silk/pitch_est_tables.c
    opus/silk/tables_pitch_lag.c
    opus/silk/encode_pulses.c
    opus/silk/PLC.c
    opus/silk/tables_pulses_per_block.c
    opus/silk/process_NLSFs.c
    opus/silk/quant_LTP_gains.c
    opus/silk/gain_quant.c
    opus/silk/resampler.c
    opus/silk/VAD.c
    opus/silk/HP_variable_cutoff.c
    opus/silk/resampler_down2_3.c
    opus/silk/VQ_WMat_EC.c
    opus/silk/init_decoder.c
    opus/silk/resampler_down2.c
    opus/silk/init_encoder.c
    opus/silk/resampler_private_AR2.c
    )
idf_component_register(SRCS "${srcs}" "${silk}" "${celt}" "${fixed}" "${float}"
                       INCLUDE_DIRS .
					                "opus/include"
									"opus/silk"
									"opus/silk/fixed"
									"opus/silk/float"
									"opus/celt"
                                    "opus/silk/fixed/mips"
									)


target_compile_definitions(${COMPONENT_TARGET} PRIVATE "-DHAVE_CONFIG_H")
