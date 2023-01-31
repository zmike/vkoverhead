; SPIR-V
; Version: 1.0
; Generator: Google spiregg; 0
; Bound: 25267
; Schema: 0
OpCapability Shader
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Fragment %5029 "MainPs" %5759 %5936 %5763 %5482
OpExecutionMode %5029 OriginUpperLeft
%2 = OpString ""
OpDecorate %5759 Location 0
OpDecorate %5936 Location 1
OpDecorate %5763 Location 4
OpDecorate %5482 Location 0
OpDecorate %5278 DescriptorSet 4
OpDecorate %5278 Binding 15
OpDecorate %5244 DescriptorSet 4
OpDecorate %5244 Binding 17
OpDecorate %3261 DescriptorSet 4
OpDecorate %3261 Binding 31
OpDecorate %3805 DescriptorSet 4
OpDecorate %3805 Binding 1
OpMemberDecorate %_struct_1026 0 Offset 0
OpMemberDecorate %_struct_1026 1 Offset 16
OpDecorate %_struct_1026 Block
OpMemberDecorate %_struct_1989 0 Offset 0
OpMemberDecorate %_struct_1989 1 Offset 16
OpMemberDecorate %_struct_1989 2 Offset 48
OpMemberDecorate %_struct_1989 3 Offset 64
OpMemberDecorate %_struct_1989 4 Offset 80
OpMemberDecorate %_struct_1989 5 Offset 96
OpMemberDecorate %_struct_1989 6 Offset 112
OpMemberDecorate %_struct_1989 7 Offset 128
OpMemberDecorate %_struct_1989 8 Offset 304
OpDecorate %_struct_1989 Block
%float = OpTypeFloat 32
%float_0_5 = OpConstant %float 0.5
%float_0 = OpConstant %float 0
%float_1 = OpConstant %float 1
%int = OpTypeInt 32 1
%int_0 = OpConstant %int 0
%int_3 = OpConstant %int 3
%v3float = OpTypeVector %float 3
%2604 = OpConstantComposite %v3float %float_0 %float_0 %float_0
%2582 = OpConstantComposite %v3float %float_1 %float_1 %float_1
%int_1 = OpConstant %int 1
%uint = OpTypeInt 32 0
%uint_3 = OpConstant %uint 3
%int_4 = OpConstant %int 4
%int_2 = OpConstant %int 2
%int_7 = OpConstant %int 7
%int_15 = OpConstant %int 15
%int_16 = OpConstant %int 16
%int_255 = OpConstant %int 255
%int_12 = OpConstant %int 12
%v4float = OpTypeVector %float 4
%1284 = OpConstantComposite %v4float %float_1 %float_1 %float_1 %float_1
%uint_0 = OpConstant %uint 0
%v2float = OpTypeVector %float 2
%1823 = OpConstantComposite %v2float %float_0 %float_0
%768 = OpConstantComposite %v2float %float_1 %float_1
%uint_1 = OpConstant %uint 1
%uint_4 = OpConstant %uint 4
%uint_2 = OpConstant %uint 2
%float_2 = OpConstant %float 2
%2243 = OpConstantComposite %v4float %float_2 %float_2 %float_2 %float_2
%2938 = OpConstantComposite %v4float %float_0 %float_0 %float_0 %float_0
%float_4 = OpConstant %float 4
%2525 = OpConstantComposite %v2float %float_2 %float_4
%float_6 = OpConstant %float 6
%float_3 = OpConstant %float 3
%float_0_212500006 = OpConstant %float 0.212500006
%float_0_715399981 = OpConstant %float 0.715399981
%float_0_0720999986 = OpConstant %float 0.0720999986
%547 = OpConstantComposite %v3float %float_0_212500006 %float_0_715399981 %float_0_0720999986
%float_0_0625 = OpConstant %float 0.0625
%326 = OpConstantComposite %v4float %float_0_5 %float_0_5 %float_0_5 %float_0_5
%1566 = OpConstantComposite %v2float %float_0_5 %float_0_5
%float_0_298999995 = OpConstant %float 0.298999995
%float_0_587000012 = OpConstant %float 0.587000012
%float_0_114 = OpConstant %float 0.114
%1268 = OpConstantComposite %v3float %float_0_298999995 %float_0_587000012 %float_0_114
%float_12_9200001 = OpConstant %float 12.9200001
%float_0_416666657 = OpConstant %float 0.416666657
%1383 = OpConstantComposite %v3float %float_0_416666657 %float_0_416666657 %float_0_416666657
%float_1_05499995 = OpConstant %float 1.05499995
%float_0_0549999997 = OpConstant %float 0.0549999997
%2237 = OpConstantComposite %v3float %float_0_0549999997 %float_0_0549999997 %float_0_0549999997
%float_0_00313080009 = OpConstant %float 0.00313080009
%508 = OpTypeSampler
%_ptr_UniformConstant_508 = OpTypePointer UniformConstant %508
%406 = OpTypeImage %float 2D 2 0 0 1 Unknown
%_ptr_UniformConstant_406 = OpTypePointer UniformConstant %406
%uint_2048 = OpConstant %uint 2048
%_arr_406_uint_2048 = OpTypeArray %406 %uint_2048
%_ptr_UniformConstant__arr_406_uint_2048 = OpTypePointer UniformConstant %_arr_406_uint_2048
%_struct_1026 = OpTypeStruct %v4float %uint
%_ptr_PushConstant__struct_1026 = OpTypePointer PushConstant %_struct_1026
%uint_15 = OpConstant %uint 15
%_arr_v3float_uint_15 = OpTypeArray %v3float %uint_15
%_arr_v2float_uint_15 = OpTypeArray %v2float %uint_15
%v2uint = OpTypeVector %uint 2
%_struct_1989 = OpTypeStruct %v4float %v4float %v4float %v4float %v4float %v4float %v4float %v4float %v2uint
%_ptr_Uniform__struct_1989 = OpTypePointer Uniform %_struct_1989
%_ptr_Input_v4float = OpTypePointer Input %v4float
%_ptr_Output_v4float = OpTypePointer Output %v4float
%void = OpTypeVoid
%1282 = OpTypeFunction %void
%bool = OpTypeBool
%_ptr_Function_float = OpTypePointer Function %float
%_ptr_Uniform_v4float = OpTypePointer Uniform %v4float
%_ptr_Uniform_float = OpTypePointer Uniform %float
%_ptr_Uniform_uint = OpTypePointer Uniform %uint
%_ptr_PushConstant_v4float = OpTypePointer PushConstant %v4float
%_ptr_PushConstant_float = OpTypePointer PushConstant %float
%v4bool = OpTypeVector %bool 4
%_ptr_PushConstant_uint = OpTypePointer PushConstant %uint
%510 = OpTypeSampledImage %406
%5278 = OpVariable %_ptr_UniformConstant_508 UniformConstant
%5244 = OpVariable %_ptr_UniformConstant_508 UniformConstant
%3261 = OpVariable %_ptr_UniformConstant__arr_406_uint_2048 UniformConstant
%6012 = OpVariable %_ptr_PushConstant__struct_1026 PushConstant
%3805 = OpVariable %_ptr_Uniform__struct_1989 Uniform
%5759 = OpVariable %_ptr_Input_v4float Input
%5936 = OpVariable %_ptr_Input_v4float Input
%5763 = OpVariable %_ptr_Input_v4float Input
%5482 = OpVariable %_ptr_Output_v4float Output
%_ptr_Function__arr_v3float_uint_15 = OpTypePointer Function %_arr_v3float_uint_15
%_ptr_Function__arr_v2float_uint_15 = OpTypePointer Function %_arr_v2float_uint_15
%1670 = OpConstantComposite %v3float %float_0 %float_1 %float_0
%1093 = OpConstantComposite %v3float %float_1 %float_0 %float_0
%2126 = OpConstantComposite %v3float %float_1 %float_0 %float_1
%1584 = OpConstantComposite %_arr_v3float_uint_15 %2604 %2604 %2604 %1670 %1093 %2126 %2604 %2604 %1670 %2604 %2604 %2604 %2604 %2604 %2604
%312 = OpConstantComposite %v2float %float_1 %float_0
%889 = OpConstantComposite %v2float %float_0 %float_1
%607 = OpConstantComposite %_arr_v2float_uint_15 %312 %1823 %889 %1823 %312 %312 %1823 %889 %1823 %312 %312 %312 %1823 %1823 %1823
%float_n0_399999976 = OpConstant %float -0.399999976
%float_0_125 = OpConstant %float 0.125
%212 = OpConstantNull %v4float
%float_0_166666672 = OpConstant %float 0.166666672
%207 = OpConstantNull %v3float
%uint_8 = OpConstant %uint 8
%uint_7 = OpConstant %uint 7
%uint_6 = OpConstant %uint 6
%uint_5 = OpConstant %uint 5
%5029 = OpFunction %void None %1282
%12154 = OpLabel
%3533 = OpVariable %_ptr_Function__arr_v2float_uint_15 Function
%3532 = OpVariable %_ptr_Function__arr_v3float_uint_15 Function
OpStore %3532 %1584
OpStore %3533 %607
%10772 = OpLoad %v4float %5759
%11401 = OpLoad %v4float %5936
%11408 = OpLoad %v4float %5763
%8323 = OpCompositeExtract %float %11408 3
%9425 = OpFDiv %float %float_1 %8323
%8942 = OpAccessChain %_ptr_Uniform_float %3805 %uint_2 %int_2
%12119 = OpLoad %float %8942
%7757 = OpConvertFToS %int %12119
%23516 = OpShiftRightArithmetic %int %7757 %int_7
%23315 = OpBitwiseAnd %int %23516 %int_1
%17654 = OpConvertSToF %float %23315
%12126 = OpExtInst %float %1 FMix %9425 %float_1 %17654
%16157 = OpVectorShuffle %v2float %10772 %10772 0 2
%22895 = OpVectorTimesScalar %v2float %16157 %12126
%16750 = OpVectorShuffle %v4float %10772 %22895 4 1 5 3
%15027 = OpVectorShuffle %v2float %16750 %16750 0 1
%11378 = OpBitwiseAnd %int %7757 %int_1
%20336 = OpINotEqual %bool %11378 %int_0
%19832 = OpCompositeConstruct %v4bool %20336 %20336 %20336 %20336
%25089 = OpAccessChain %_ptr_PushConstant_uint %6012 %int_1
%10936 = OpLoad %uint %25089
%22039 = OpAccessChain %_ptr_UniformConstant_406 %3261 %10936
%6465 = OpLoad %406 %22039
%12838 = OpLoad %508 %5244
%13901 = OpSampledImage %510 %6465 %12838
%22927 = OpImageSampleImplicitLod %v4float %13901 %15027 None
%6558 = OpLoad %508 %5278
%17551 = OpSampledImage %510 %6465 %6558
%22915 = OpImageSampleImplicitLod %v4float %17551 %15027 None
%14733 = OpSelect %v4float %19832 %22927 %22915
%17844 = OpAccessChain %_ptr_Uniform_uint %3805 %uint_8 %int_1
%13712 = OpLoad %uint %17844
%7481 = OpBitcast %int %13712
%19977 = OpShiftRightArithmetic %int %7481 %int_16
%20404 = OpBitwiseAnd %int %19977 %int_15
%23505 = OpBitcast %uint %20404
%11108 = OpAccessChain %_ptr_Uniform_uint %3805 %uint_8 %int_0
%6552 = OpLoad %uint %11108
%6789 = OpBitcast %int %6552
%6276 = OpBitwiseAnd %int %6789 %int_255
%15421 = OpBitwiseAnd %int %6276 %int_15
%6863 = OpBitcast %uint %15421
%21805 = OpShiftRightArithmetic %int %6276 %int_4
%20405 = OpBitwiseAnd %int %21805 %int_15
%23506 = OpBitcast %uint %20405
%9987 = OpAccessChain %_ptr_Uniform_float %3805 %uint_7 %int_1
%14137 = OpLoad %float %9987
%22115 = OpAccessChain %_ptr_Uniform_float %3805 %uint_7 %int_2
%24369 = OpLoad %float %22115
%24585 = OpAccessChain %_ptr_Uniform_float %3805 %uint_7 %int_3
%23572 = OpLoad %float %24585
%9279 = OpShiftRightArithmetic %int %7757 %int_12
%7065 = OpBitwiseAnd %int %9279 %int_1
%19242 = OpIEqual %bool %7065 %int_1
OpSelectionMerge %22453 None
OpSwitch %uint_0 %11824
%11824 = OpLabel
%18701 = OpIEqual %bool %23505 %uint_0
OpSelectionMerge %21781 None
OpBranchConditional %18701 %21780 %18530
%18530 = OpLabel
%21123 = OpIEqual %bool %23505 %uint_1
OpSelectionMerge %16741 None
OpBranchConditional %21123 %10519 %18531
%18531 = OpLabel
%21124 = OpIEqual %bool %23505 %uint_3
OpSelectionMerge %22590 None
OpBranchConditional %21124 %10518 %18532
%18532 = OpLabel
%21125 = OpIEqual %bool %23505 %uint_4
OpSelectionMerge %22589 None
OpBranchConditional %21125 %10517 %18533
%18533 = OpLabel
%21126 = OpIEqual %bool %23505 %uint_2
OpSelectionMerge %16487 None
OpBranchConditional %21126 %23296 %16487
%23296 = OpLabel
%23325 = OpBitcast %int %6863
%11894 = OpVectorShuffle %v3float %14733 %14733 0 1 2
%23132 = OpVectorShuffle %v3float %14733 %14733 3 3 3
%21103 = OpFMul %v3float %11894 %23132
%12931 = OpCompositeExtract %float %21103 0
%12798 = OpCompositeExtract %float %21103 1
%22104 = OpCompositeExtract %float %21103 2
%13745 = OpCompositeConstruct %v4float %12931 %12798 %22104 %float_1
%10902 = OpAccessChain %_ptr_Function_float %3533 %23325 %int_0
%7456 = OpLoad %float %10902
%8487 = OpCompositeConstruct %v4float %7456 %7456 %7456 %7456
%23611 = OpExtInst %v4float %1 FMix %13745 %14733 %8487
%20277 = OpVectorShuffle %v4float %14733 %14733 3 3 3 3
%8475 = OpAccessChain %_ptr_Function_float %3533 %23325 %int_1
%22492 = OpLoad %float %8475
%8488 = OpCompositeConstruct %v4float %22492 %22492 %22492 %22492
%25074 = OpExtInst %v4float %1 FMix %23611 %20277 %8488
%25122 = OpVectorShuffle %v3float %25074 %25074 0 1 2
%17379 = OpVectorTimesScalar %v3float %25122 %float_12_9200001
%19379 = OpExtInst %v3float %1 Pow %25122 %1383
%11103 = OpVectorTimesScalar %v3float %19379 %float_1_05499995
%7485 = OpFSub %v3float %11103 %2237
%10214 = OpCompositeExtract %float %25074 0
%24433 = OpFOrdLessThanEqual %bool %10214 %float_0_00313080009
%20392 = OpCompositeExtract %float %17379 0
%6437 = OpCompositeExtract %float %7485 0
%8911 = OpSelect %float %24433 %20392 %6437
%11697 = OpCompositeExtract %float %25074 1
%10181 = OpFOrdLessThanEqual %bool %11697 %float_0_00313080009
%20393 = OpCompositeExtract %float %17379 1
%6438 = OpCompositeExtract %float %7485 1
%8912 = OpSelect %float %10181 %20393 %6438
%11698 = OpCompositeExtract %float %25074 2
%10182 = OpFOrdLessThanEqual %bool %11698 %float_0_00313080009
%20394 = OpCompositeExtract %float %17379 2
%6418 = OpCompositeExtract %float %7485 2
%8324 = OpSelect %float %10182 %20394 %6418
%7739 = OpCompositeConstruct %v3float %8911 %8912 %8324
%21312 = OpDot %float %7739 %547
%17241 = OpCompositeExtract %float %22895 0
%23444 = OpFAdd %float %21312 %17241
%13001 = OpCompositeExtract %float %22895 1
%22169 = OpFMul %float %23444 %13001
%15287 = OpCompositeExtract %float %10772 1
%18974 = OpCompositeConstruct %v2float %22169 %15287
%18200 = OpConvertSToF %float %11378
OpSelectionMerge %15026 None
OpSwitch %uint_0 %10094
%10094 = OpLabel
%13055 = OpFOrdEqual %bool %18200 %float_1
OpSelectionMerge %23614 None
OpBranchConditional %13055 %10951 %23614
%10951 = OpLabel
%18755 = OpSampledImage %510 %6465 %12838
%24842 = OpImageSampleImplicitLod %v4float %18755 %18974 None
OpBranch %15026
%23614 = OpLabel
%14720 = OpSampledImage %510 %6465 %6558
%24843 = OpImageSampleImplicitLod %v4float %14720 %18974 None
OpBranch %15026
%15026 = OpLabel
%20940 = OpPhi %v4float %24842 %10951 %24843 %23614
%10758 = OpVectorShuffle %v3float %20940 %20940 0 1 2
%11043 = OpDot %float %10758 %1268
%19793 = OpCompositeInsert %v4float %11043 %20940 3
%14427 = OpCompositeExtract %float %14733 3
%21518 = OpCompositeInsert %v4float %14427 %20940 3
%13115 = OpLoad %float %8475
%15742 = OpCompositeConstruct %v4float %13115 %13115 %13115 %13115
%16749 = OpExtInst %v4float %1 FMix %21518 %19793 %15742
OpBranch %22453
%16487 = OpLabel
OpBranch %22589
%10517 = OpLabel
%24163 = OpCompositeExtract %float %22895 1
%8088 = OpFMul %float %24369 %24163
%19281 = OpExtInst %float %1 Fract %8088
%21947 = OpFSub %float %float_1 %19281
%12300 = OpExtInst %float %1 Fma %float_n0_399999976 %21947 %float_1
%19068 = OpFDiv %float %19281 %12300
%15873 = OpExtInst %float %1 FMix %float_4 %float_1 %19068
%23806 = OpExtInst %float %1 FMix %float_1 %float_0_0625 %19068
%22204 = OpFSub %v4float %16750 %326
%20365 = OpVectorTimesScalar %v4float %22204 %15873
%17629 = OpFAdd %v4float %20365 %326
%16992 = OpVectorTimesScalar %v4float %22204 %23806
%13062 = OpFAdd %v4float %16992 %326
%9998 = OpCompositeExtract %float %10772 1
%20258 = OpCompositeInsert %v4float %9998 %17629 1
%14709 = OpCompositeInsert %v4float %9998 %13062 1
%21300 = OpVectorShuffle %v2float %20258 %20258 0 1
%22484 = OpConvertSToF %float %11378
OpSelectionMerge %15028 None
OpSwitch %uint_0 %10095
%10095 = OpLabel
%13056 = OpFOrdEqual %bool %22484 %float_1
OpSelectionMerge %23615 None
OpBranchConditional %13056 %10952 %23615
%10952 = OpLabel
%18756 = OpSampledImage %510 %6465 %12838
%24844 = OpImageSampleImplicitLod %v4float %18756 %21300 None
OpBranch %15028
%23615 = OpLabel
%14721 = OpSampledImage %510 %6465 %6558
%24845 = OpImageSampleImplicitLod %v4float %14721 %21300 None
OpBranch %15028
%15028 = OpLabel
%22821 = OpPhi %v4float %24844 %10952 %24845 %23615
%15913 = OpVectorShuffle %v2float %14709 %14709 0 1
OpSelectionMerge %16052 None
OpSwitch %uint_0 %17631
%17631 = OpLabel
OpSelectionMerge %23616 None
OpBranchConditional %13056 %10953 %23616
%10953 = OpLabel
%18757 = OpSampledImage %510 %6465 %12838
%24846 = OpImageSampleImplicitLod %v4float %18757 %15913 None
OpBranch %16052
%23616 = OpLabel
%14722 = OpSampledImage %510 %6465 %6558
%24847 = OpImageSampleImplicitLod %v4float %14722 %15913 None
OpBranch %16052
%16052 = OpLabel
%10306 = OpPhi %v4float %24846 %10953 %24847 %23616
%10992 = OpFMul %float %19068 %19068
%11584 = OpCompositeConstruct %v4float %10992 %10992 %10992 %10992
%10910 = OpExtInst %v4float %1 FMix %10306 %22821 %11584
%21391 = OpCompositeExtract %float %14733 3
%18273 = OpCompositeExtract %float %10910 3
%15602 = OpBitcast %int %6863
%13598 = OpAccessChain %_ptr_Function_float %3532 %15602 %int_1
%15019 = OpLoad %float %13598
%10053 = OpFOrdNotEqual %bool %15019 %float_1
%25204 = OpSelect %float %10053 %float_1 %float_0
%23627 = OpExtInst %float %1 FMix %21391 %18273 %25204
%10389 = OpCompositeExtract %float %10772 3
%23122 = OpFMul %float %23572 %10389
%9366 = OpFMul %float %23122 %float_0_125
%14021 = OpVectorShuffle %v2float %10910 %10910 0 1
%13662 = OpFSub %v2float %14021 %1566
%20736 = OpVectorTimesScalar %v2float %13662 %float_2
%23037 = OpFMul %float %9366 %23627
%12347 = OpVectorTimesScalar %v2float %20736 %23037
%25151 = OpFSub %v2float %15027 %12347
OpSelectionMerge %18256 None
OpSwitch %uint_0 %17632
%17632 = OpLabel
OpSelectionMerge %23617 None
OpBranchConditional %13056 %10954 %23617
%10954 = OpLabel
%18758 = OpSampledImage %510 %6465 %12838
%24848 = OpImageSampleImplicitLod %v4float %18758 %25151 None
OpBranch %18256
%23617 = OpLabel
%14723 = OpSampledImage %510 %6465 %6558
%24849 = OpImageSampleImplicitLod %v4float %14723 %25151 None
OpBranch %18256
%18256 = OpLabel
%12501 = OpPhi %v4float %24848 %10954 %24849 %23617
OpBranch %22453
%22589 = OpLabel
OpBranch %22590
%10518 = OpLabel
%7381 = OpCompositeExtract %float %10772 3
%9344 = OpFMul %float %23572 %7381
%9367 = OpFMul %float %9344 %float_0_125
%14022 = OpVectorShuffle %v2float %14733 %14733 0 1
%12674 = OpFSub %v2float %14022 %1566
%10490 = OpVectorTimesScalar %v2float %12674 %float_2
%18663 = OpCompositeExtract %float %14733 3
%13384 = OpFMul %float %9367 %18663
%15151 = OpVectorTimesScalar %v2float %10490 %13384
%10461 = OpFSub %v2float %15027 %15151
%17984 = OpConvertSToF %float %11378
OpSelectionMerge %18257 None
OpSwitch %uint_0 %10096
%10096 = OpLabel
%13057 = OpFOrdEqual %bool %17984 %float_1
OpSelectionMerge %23618 None
OpBranchConditional %13057 %10955 %23618
%10955 = OpLabel
%18759 = OpSampledImage %510 %6465 %12838
%24850 = OpImageSampleImplicitLod %v4float %18759 %10461 None
OpBranch %18257
%23618 = OpLabel
%14724 = OpSampledImage %510 %6465 %6558
%24851 = OpImageSampleImplicitLod %v4float %14724 %10461 None
OpBranch %18257
%18257 = OpLabel
%12502 = OpPhi %v4float %24850 %10955 %24851 %23618
OpBranch %22453
%22590 = OpLabel
OpBranch %16741
%10519 = OpLabel
%24164 = OpCompositeExtract %float %22895 1
%8089 = OpFMul %float %24369 %24164
%19282 = OpExtInst %float %1 Fract %8089
%21948 = OpFSub %float %float_1 %19282
%12301 = OpExtInst %float %1 Fma %float_n0_399999976 %21948 %float_1
%19069 = OpFDiv %float %19282 %12301
%15874 = OpExtInst %float %1 FMix %float_4 %float_1 %19069
%23807 = OpExtInst %float %1 FMix %float_1 %float_0_0625 %19069
%22205 = OpFSub %v4float %16750 %326
%20366 = OpVectorTimesScalar %v4float %22205 %15874
%17630 = OpFAdd %v4float %20366 %326
%16993 = OpVectorTimesScalar %v4float %22205 %23807
%13063 = OpFAdd %v4float %16993 %326
%9999 = OpCompositeExtract %float %10772 1
%20259 = OpCompositeInsert %v4float %9999 %17630 1
%14710 = OpCompositeInsert %v4float %9999 %13063 1
%21301 = OpVectorShuffle %v2float %20259 %20259 0 1
%22485 = OpConvertSToF %float %11378
OpSelectionMerge %15029 None
OpSwitch %uint_0 %10097
%10097 = OpLabel
%13058 = OpFOrdEqual %bool %22485 %float_1
OpSelectionMerge %23619 None
OpBranchConditional %13058 %10956 %23619
%10956 = OpLabel
%18760 = OpSampledImage %510 %6465 %12838
%24852 = OpImageSampleImplicitLod %v4float %18760 %21301 None
OpBranch %15029
%23619 = OpLabel
%14725 = OpSampledImage %510 %6465 %6558
%24853 = OpImageSampleImplicitLod %v4float %14725 %21301 None
OpBranch %15029
%15029 = OpLabel
%22822 = OpPhi %v4float %24852 %10956 %24853 %23619
%15914 = OpVectorShuffle %v2float %14710 %14710 0 1
OpSelectionMerge %16053 None
OpSwitch %uint_0 %17633
%17633 = OpLabel
OpSelectionMerge %23620 None
OpBranchConditional %13058 %10957 %23620
%10957 = OpLabel
%18761 = OpSampledImage %510 %6465 %12838
%24854 = OpImageSampleImplicitLod %v4float %18761 %15914 None
OpBranch %16053
%23620 = OpLabel
%14726 = OpSampledImage %510 %6465 %6558
%24855 = OpImageSampleImplicitLod %v4float %14726 %15914 None
OpBranch %16053
%16053 = OpLabel
%10307 = OpPhi %v4float %24854 %10957 %24855 %23620
%10993 = OpFMul %float %19069 %19069
%11585 = OpCompositeConstruct %v4float %10993 %10993 %10993 %10993
%10911 = OpExtInst %v4float %1 FMix %10307 %22822 %11585
%21392 = OpCompositeExtract %float %14733 3
%18274 = OpCompositeExtract %float %10911 3
%15603 = OpBitcast %int %6863
%13599 = OpAccessChain %_ptr_Function_float %3532 %15603 %int_1
%15020 = OpLoad %float %13599
%10054 = OpFOrdNotEqual %bool %15020 %float_1
%25223 = OpSelect %float %10054 %float_1 %float_0
%6587 = OpExtInst %float %1 FMix %21392 %18274 %25223
%22455 = OpCompositeInsert %v4float %6587 %10911 3
OpBranch %22453
%16741 = OpLabel
OpBranch %21781
%21780 = OpLabel
OpBranch %22453
%21781 = OpLabel
OpBranch %22453
%22453 = OpLabel
%12885 = OpPhi %v4float %14733 %21780 %22455 %16053 %12502 %18257 %12501 %18256 %16749 %15026 %14733 %21781
OpSelectionMerge %10418 None
OpBranchConditional %19242 %12690 %10418
%12690 = OpLabel
%13620 = OpCompositeExtract %float %10772 3
%11558 = OpFMul %float %14137 %13620
OpBranch %10418
%10418 = OpLabel
%12904 = OpPhi %float %14137 %22453 %11558 %12690
OpSelectionMerge %17414 None
OpSwitch %6863 %17414 0 %12548 1 %17553 2 %8028 3 %23580 4 %23907 5 %25181 6 %25180 7 %23870 8 %23869 9 %23927 10 %23926 11 %24735 12 %8838 13 %8837 14 %8534
%8534 = OpLabel
%7497 = OpVectorShuffle %v4float %12885 %12885 2 2 2 2
OpBranch %17414
%8837 = OpLabel
%24955 = OpVectorShuffle %v4float %12885 %12885 1 1 1 1
OpBranch %17414
%8838 = OpLabel
%24956 = OpVectorShuffle %v4float %12885 %12885 0 0 0 0
OpBranch %17414
%24735 = OpLabel
%15723 = OpVectorShuffle %v4float %12885 %12885 6 6 6 3
%16794 = OpCompositeInsert %v4float %float_1 %15723 3
OpBranch %17414
%23926 = OpLabel
%15724 = OpVectorShuffle %v4float %12885 %12885 5 5 5 3
%16795 = OpCompositeInsert %v4float %float_1 %15724 3
OpBranch %17414
%23927 = OpLabel
%15725 = OpVectorShuffle %v4float %12885 %12885 4 4 4 3
%16796 = OpCompositeInsert %v4float %float_1 %15725 3
OpBranch %17414
%23869 = OpLabel
%14323 = OpVectorShuffle %v4float %12885 %2582 4 5 6 3
%17550 = OpVectorShuffle %v3float %12885 %12885 0 1 2
%9196 = OpDot %float %17550 %1268
%12954 = OpCompositeInsert %v4float %9196 %14323 3
OpBranch %17414
%23870 = OpLabel
%14324 = OpVectorShuffle %v4float %12885 %2582 4 5 6 3
%17552 = OpVectorShuffle %v3float %12885 %12885 0 1 2
%9197 = OpDot %float %17552 %1268
%12955 = OpCompositeInsert %v4float %9197 %14324 3
OpBranch %17414
%25180 = OpLabel
%20203 = OpVectorShuffle %v3float %12885 %12885 0 1 2
%19883 = OpDot %float %20203 %1268
%12956 = OpCompositeInsert %v4float %19883 %12885 3
OpBranch %17414
%25181 = OpLabel
%20165 = OpVectorShuffle %v3float %12885 %12885 0 1 2
%15772 = OpDot %float %20165 %1268
%21543 = OpCompositeConstruct %v3float %15772 %15772 %15772
%13687 = OpExtInst %v3float %1 FMix %2582 %20165 %21543
%20908 = OpVectorShuffle %v4float %12885 %13687 4 5 6 3
%11501 = OpCompositeInsert %v4float %float_1 %20908 3
OpBranch %17414
%23907 = OpLabel
%12685 = OpVectorShuffle %v3float %12885 %12885 0 1 2
%11644 = OpCompositeExtract %float %12885 3
%20655 = OpCompositeConstruct %v3float %11644 %11644 %11644
%8079 = OpExtInst %v3float %1 FMix %2582 %12685 %20655
%20909 = OpVectorShuffle %v4float %12885 %8079 4 5 6 3
%11502 = OpCompositeInsert %v4float %float_1 %20909 3
OpBranch %17414
%23580 = OpLabel
%18408 = OpCompositeExtract %float %12885 3
%16296 = OpCompositeInsert %v4float %18408 %12885 3
OpBranch %17414
%8028 = OpLabel
%24957 = OpVectorShuffle %v4float %12885 %2582 4 5 6 3
OpBranch %17414
%17553 = OpLabel
OpBranch %17414
%12548 = OpLabel
%19447 = OpCompositeInsert %v4float %float_1 %12885 3
OpBranch %17414
%17414 = OpLabel
%12905 = OpPhi %v4float %12885 %10418 %7497 %8534 %24955 %8837 %24956 %8838 %16794 %24735 %16795 %23926 %16796 %23927 %12954 %23869 %12955 %23870 %12956 %25180 %11501 %25181 %11502 %23907 %16296 %23580 %24957 %8028 %12885 %17553 %19447 %12548
OpSelectionMerge %19280 None
OpSwitch %23506 %19280 0 %6519 3 %9840 4 %11107 1 %22796 2 %7318 5 %17225 6 %6615
%6615 = OpLabel
%22956 = OpVectorShuffle %v3float %12905 %12905 0 1 2
%6273 = OpDot %float %22956 %547
%21121 = OpVectorTimesScalar %v3float %2582 %6273
%21020 = OpVectorShuffle %v4float %12905 %21121 4 5 6 3
OpBranch %19280
%17225 = OpLabel
%15255 = OpFAdd %v4float %1284 %12905
%13867 = OpVectorTimesScalar %v4float %15255 %float_0_5
OpBranch %19280
%7318 = OpLabel
OpBranch %19280
%22796 = OpLabel
%21166 = OpFMul %v4float %12905 %2243
OpBranch %19280
%11107 = OpLabel
%18481 = OpFSub %v4float %1284 %12905
OpBranch %19280
%9840 = OpLabel
%15796 = OpFAdd %v4float %1284 %12905
OpBranch %19280
%6519 = OpLabel
OpBranch %19280
%19280 = OpLabel
%18229 = OpPhi %v4float %12905 %17414 %21020 %6615 %13867 %17225 %12905 %7318 %21166 %22796 %18481 %11107 %15796 %9840 %12905 %6519
%17361 = OpCompositeConstruct %v4float %12904 %12904 %12904 %12904
%20640 = OpExtInst %v4float %1 FMix %1284 %18229 %17361
%24406 = OpExtInst %v4float %1 FClamp %20640 %2938 %1284
%8284 = OpVectorShuffle %v3float %11401 %11401 0 1 2
%10505 = OpVectorShuffle %v3float %24406 %24406 0 1 2
%21104 = OpFMul %v3float %10505 %8284
%12932 = OpCompositeExtract %float %21104 0
%11546 = OpCompositeExtract %float %21104 1
%16477 = OpCompositeExtract %float %21104 2
%6610 = OpExtInst %float %1 FMax %11546 %16477
%10849 = OpExtInst %float %1 FMax %12932 %6610
%10826 = OpExtInst %float %1 FMax %10849 %float_1
%6285 = OpVectorShuffle %v3float %21104 %212 0 1 2
%15268 = OpCompositeConstruct %v3float %10826 %10826 %10826
%18389 = OpFDiv %v3float %6285 %15268
%7175 = OpExtInst %v3float %1 FClamp %18389 %2604 %2582
%12013 = OpVectorShuffle %v3float %7175 %212 0 1 2
%22186 = OpCompositeExtract %float %7175 0
%6734 = OpCompositeExtract %float %7175 1
%21520 = OpCompositeExtract %float %7175 2
%10879 = OpExtInst %float %1 FMin %6734 %21520
%22069 = OpExtInst %float %1 FMin %22186 %10879
%9407 = OpExtInst %float %1 FMax %6734 %21520
%16611 = OpExtInst %float %1 FMax %22186 %9407
%12953 = OpFSub %float %16611 %22069
%6726 = OpCompositeInsert %v3float %16611 %2604 2
%9693 = OpFOrdNotEqual %bool %12953 %float_0
OpSelectionMerge %13956 None
OpBranchConditional %9693 %21220 %13956
%21220 = OpLabel
%21352 = OpFDiv %float %12953 %16611
%24131 = OpVectorShuffle %v3float %6726 %6726 2 2 2
%23636 = OpFSub %v3float %24131 %12013
%18526 = OpCompositeConstruct %v3float %12953 %12953 %12953
%19838 = OpFDiv %v3float %23636 %18526
%17460 = OpVectorShuffle %v3float %19838 %19838 2 0 1
%23676 = OpFSub %v3float %19838 %17460
%10968 = OpVectorShuffle %v2float %23676 %23676 0 1
%22297 = OpFAdd %v2float %10968 %2525
%10791 = OpFOrdGreaterThanEqual %bool %22186 %16611
OpSelectionMerge %19104 None
OpBranchConditional %10791 %10319 %15070
%15070 = OpLabel
%9831 = OpFOrdGreaterThanEqual %bool %6734 %16611
OpSelectionMerge %22296 None
OpBranchConditional %9831 %9315 %11683
%11683 = OpLabel
%22789 = OpCompositeExtract %float %22297 1
%13611 = OpCompositeConstruct %v3float %22789 %21352 %16611
OpBranch %22296
%9315 = OpLabel
%18754 = OpCompositeExtract %float %22297 0
%13612 = OpCompositeConstruct %v3float %18754 %21352 %16611
OpBranch %22296
%22296 = OpLabel
%12503 = OpPhi %v3float %13611 %11683 %13612 %9315
OpBranch %19104
%10319 = OpLabel
%18762 = OpCompositeExtract %float %23676 2
%13613 = OpCompositeConstruct %v3float %18762 %21352 %16611
OpBranch %19104
%19104 = OpLabel
%20309 = OpPhi %v3float %12503 %22296 %13613 %10319
%14718 = OpCompositeExtract %float %20309 0
%7966 = OpFMul %float %14718 %float_0_166666672
%13140 = OpExtInst %float %1 Fract %7966
%11027 = OpCompositeInsert %v3float %13140 %20309 0
OpBranch %13956
%13956 = OpLabel
%21785 = OpPhi %v3float %6726 %19280 %11027 %19104
%24994 = OpAccessChain %_ptr_Uniform_v4float %3805 %uint_6
%18382 = OpAccessChain %_ptr_Uniform_float %3805 %uint_6 %int_0
%7048 = OpLoad %float %18382
%7519 = OpCompositeExtract %float %21785 0
%20013 = OpFAdd %float %7519 %7048
%24653 = OpFAdd %float %float_1 %20013
%10994 = OpFOrdGreaterThanEqual %bool %20013 %float_0
%15327 = OpSelect %float %10994 %20013 %24653
%17517 = OpFSub %float %15327 %float_1
%9140 = OpFOrdGreaterThanEqual %bool %17517 %float_0
%7870 = OpSelect %float %9140 %17517 %15327
%23776 = OpLoad %v4float %24994
%13795 = OpVectorShuffle %v2float %23776 %23776 1 2
%14255 = OpVectorShuffle %v2float %21785 %21785 1 2
%20181 = OpFMul %v2float %14255 %13795
%19746 = OpVectorShuffle %v2float %20181 %207 0 1
%17185 = OpExtInst %v2float %1 FClamp %19746 %1823 %768
%9932 = OpVectorShuffle %v3float %17185 %207 1 1 1
%7867 = OpCompositeExtract %float %17185 0
%6746 = OpFOrdNotEqual %bool %7867 %float_0
OpSelectionMerge %20070 None
OpBranchConditional %6746 %20474 %20070
%20474 = OpLabel
%11331 = OpFMul %float %7870 %float_6
%13270 = OpExtInst %float %1 Floor %11331
%8027 = OpCompositeExtract %float %17185 1
%19736 = OpFSub %float %float_1 %7867
%18420 = OpFMul %float %8027 %19736
%12950 = OpFNegate %float %13270
%8995 = OpExtInst %float %1 Fma %7870 %float_6 %12950
%8920 = OpFNegate %float %7867
%24833 = OpExtInst %float %1 Fma %8920 %8995 %float_1
%14669 = OpFMul %float %8027 %24833
%15641 = OpFSub %float %float_1 %8995
%22717 = OpExtInst %float %1 Fma %8920 %15641 %float_1
%7376 = OpFMul %float %8027 %22717
%15844 = OpFOrdEqual %bool %13270 %float_0
OpSelectionMerge %22302 None
OpBranchConditional %15844 %13706 %16800
%16800 = OpLabel
%15477 = OpFOrdEqual %bool %13270 %float_1
OpSelectionMerge %22301 None
OpBranchConditional %15477 %13705 %16801
%16801 = OpLabel
%15478 = OpFOrdEqual %bool %13270 %float_2
OpSelectionMerge %22300 None
OpBranchConditional %15478 %13704 %16802
%16802 = OpLabel
%15479 = OpFOrdEqual %bool %13270 %float_3
OpSelectionMerge %22299 None
OpBranchConditional %15479 %13703 %16803
%16803 = OpLabel
%15480 = OpFOrdEqual %bool %13270 %float_4
OpSelectionMerge %22298 None
OpBranchConditional %15480 %12699 %15067
%15067 = OpLabel
%20797 = OpCompositeConstruct %v3float %8027 %18420 %14669
OpBranch %22298
%12699 = OpLabel
%16762 = OpCompositeConstruct %v3float %7376 %18420 %8027
OpBranch %22298
%22298 = OpLabel
%12504 = OpPhi %v3float %16762 %12699 %20797 %15067
OpBranch %22299
%13703 = OpLabel
%16763 = OpCompositeConstruct %v3float %18420 %14669 %8027
OpBranch %22299
%22299 = OpLabel
%12505 = OpPhi %v3float %16763 %13703 %12504 %22298
OpBranch %22300
%13704 = OpLabel
%16764 = OpCompositeConstruct %v3float %18420 %8027 %7376
OpBranch %22300
%22300 = OpLabel
%12506 = OpPhi %v3float %16764 %13704 %12505 %22299
OpBranch %22301
%13705 = OpLabel
%16765 = OpCompositeConstruct %v3float %14669 %8027 %18420
OpBranch %22301
%22301 = OpLabel
%12507 = OpPhi %v3float %16765 %13705 %12506 %22300
OpBranch %22302
%13706 = OpLabel
%16766 = OpCompositeConstruct %v3float %8027 %7376 %18420
OpBranch %22302
%22302 = OpLabel
%12508 = OpPhi %v3float %16766 %13706 %12507 %22301
OpBranch %20070
%20070 = OpLabel
%20826 = OpPhi %v3float %9932 %13956 %12508 %22302
%11739 = OpVectorShuffle %v3float %20826 %212 0 1 2
%22578 = OpVectorTimesScalar %v3float %11739 %10826
%15853 = OpVectorShuffle %v4float %24406 %22578 4 5 6 3
%24173 = OpCompositeExtract %float %24406 3
%9707 = OpIEqual %bool %23315 %int_0
OpSelectionMerge %20071 None
OpBranchConditional %9707 %14090 %20071
%14090 = OpLabel
%20171 = OpAccessChain %_ptr_Uniform_float %3805 %int_0 %int_3
%11131 = OpLoad %float %20171
%24376 = OpAccessChain %_ptr_Uniform_float %3805 %int_0 %int_2
%7316 = OpLoad %float %24376
%22791 = OpFOrdLessThan %bool %11131 %7316
OpSelectionMerge %13957 DontFlatten
OpBranchConditional %22791 %11167 %13957
%11167 = OpLabel
%14607 = OpExtInst %float %1 SmoothStep %11131 %7316 %24173
%11897 = OpCompositeInsert %v4float %14607 %15853 3
OpBranch %13957
%13957 = OpLabel
%21709 = OpPhi %v4float %15853 %14090 %11897 %11167
%9066 = OpAccessChain %_ptr_Uniform_float %3805 %uint_5 %int_0
%9462 = OpLoad %float %9066
%20599 = OpFOrdGreaterThanEqual %bool %24173 %9462
%18090 = OpAccessChain %_ptr_Uniform_float %3805 %uint_5 %int_3
%21973 = OpLoad %float %18090
%22873 = OpFOrdLessThanEqual %bool %24173 %21973
%22711 = OpLogicalAnd %bool %20599 %22873
OpSelectionMerge %17198 DontFlatten
OpBranchConditional %22711 %14091 %17198
%14091 = OpLabel
%22508 = OpAccessChain %_ptr_Uniform_float %3805 %uint_5 %int_2
%12457 = OpLoad %float %22508
%21704 = OpFOrdLessThanEqual %bool %24173 %12457
OpSelectionMerge %18030 None
OpBranchConditional %21704 %11202 %14092
%14092 = OpLabel
%19213 = OpAccessChain %_ptr_Uniform_float %3805 %uint_5 %int_1
%11381 = OpLoad %float %19213
%6712 = OpExtInst %float %1 SmoothStep %21973 %11381 %24173
OpBranch %18030
%11202 = OpLabel
%7190 = OpExtInst %float %1 SmoothStep %9462 %12457 %24173
OpBranch %18030
%18030 = OpLabel
%21710 = OpPhi %float %7190 %11202 %6712 %14092
%6976 = OpAccessChain %_ptr_Uniform_v4float %3805 %uint_4
%8460 = OpLoad %v4float %6976
%11717 = OpCompositeConstruct %v4float %21710 %21710 %21710 %21710
%16751 = OpExtInst %v4float %1 FMix %21709 %8460 %11717
OpBranch %17198
%17198 = OpLabel
%12509 = OpPhi %v4float %21709 %13957 %16751 %18030
OpBranch %20071
%20071 = OpLabel
%20941 = OpPhi %v4float %15853 %20070 %12509 %17198
%10720 = OpVectorShuffle %v3float %20941 %20941 0 1 2
%11085 = OpDot %float %10720 %547
%19553 = OpCompositeConstruct %v3float %11085 %11085 %11085
%15457 = OpAccessChain %_ptr_PushConstant_v4float %6012 %int_0
%17359 = OpAccessChain %_ptr_PushConstant_float %6012 %int_0 %int_3
%24115 = OpLoad %float %17359
%8145 = OpCompositeConstruct %v3float %24115 %24115 %24115
%7920 = OpExtInst %v3float %1 FMix %10720 %19553 %8145
%19560 = OpLoad %v4float %15457
%17331 = OpVectorShuffle %v3float %19560 %19560 0 1 2
%14256 = OpVectorShuffle %v3float %7920 %212 0 1 2
%21145 = OpFMul %v3float %17331 %14256
%12634 = OpVectorShuffle %v4float %20941 %21145 4 5 6 3
%20823 = OpAccessChain %_ptr_Uniform_float %3805 %uint_3 %int_3
%23280 = OpLoad %float %20823
%17299 = OpCompositeExtract %float %20941 3
%25266 = OpAccessChain %_ptr_Uniform_float %3805 %uint_7 %int_0
%15336 = OpLoad %float %25266
%18664 = OpExtInst %float %1 Fma %23280 %17299 %15336
%7848 = OpExtInst %float %1 FClamp %18664 %float_0 %float_1
%12800 = OpIEqual %bool %23315 %int_1
OpSelectionMerge %14260 None
OpBranchConditional %12800 %24473 %12691
%12691 = OpLabel
%10447 = OpCompositeExtract %float %11401 3
%21397 = OpFMul %float %7848 %10447
%18602 = OpCompositeInsert %v4float %21397 %12634 3
OpBranch %14260
%24473 = OpLabel
%6781 = OpCompositeExtract %float %11408 2
%15396 = OpFAdd %float %6781 %8323
%23960 = OpAccessChain %_ptr_Uniform_float %3805 %int_0 %int_2
%23016 = OpLoad %float %23960
%24526 = OpFAdd %float %7848 %23016
%18561 = OpAccessChain %_ptr_Uniform_float %3805 %uint_5 %int_0
%9016 = OpLoad %float %18561
%16415 = OpExtInst %float %1 FMax %9016 %7848
%11718 = OpFSub %float %15396 %7848
%9296 = OpFSub %float %24526 %7848
%21626 = OpFDiv %float %11718 %9296
%9604 = OpExtInst %float %1 FClamp %21626 %float_0 %float_1
%10612 = OpFMul %float %9604 %16415
%15998 = OpAccessChain %_ptr_Uniform_float %3805 %int_0 %int_3
%8080 = OpLoad %float %15998
%17123 = OpFSub %float %7848 %8080
%14133 = OpFSub %float %6781 %17123
%22211 = OpFSub %float %7848 %17123
%21512 = OpFDiv %float %14133 %22211
%9685 = OpExtInst %float %1 FClamp %21512 %float_0 %float_1
%9373 = OpFNegate %float %16415
%8269 = OpExtInst %float %1 Fma %9685 %9373 %16415
%22798 = OpExtInst %float %1 FMin %10612 %8269
%19241 = OpAccessChain %_ptr_Uniform_float %3805 %uint_5 %int_1
%14727 = OpLoad %float %19241
%11412 = OpCompositeExtract %float %11401 3
%19246 = OpExtInst %float %1 FMax %14727 %11412
%9172 = OpFMul %float %22798 %19246
%13727 = OpCompositeInsert %v4float %9172 %12634 3
OpBranch %14260
%14260 = OpLabel
%19283 = OpPhi %v4float %18602 %12691 %13727 %24473
%24093 = OpCompositeExtract %float %19283 3
%19596 = OpVectorShuffle %v3float %19283 %19283 0 1 2
%9574 = OpExtInst %v3float %1 FClamp %19596 %2604 %2582
%12623 = OpAccessChain %_ptr_Uniform_float %3805 %int_1 %int_3
%8967 = OpLoad %float %12623
%8655 = OpFNegate %float %8967
%8186 = OpExtInst %float %1 FClamp %8655 %float_0 %float_1
%16474 = OpExtInst %float %1 FMix %24093 %float_1 %8186
%23400 = OpVectorShuffle %v3float %9574 %212 0 1 2
%21627 = OpVectorTimesScalar %v3float %23400 %16474
%14002 = OpVectorShuffle %v4float %19283 %21627 4 5 6 3
%15305 = OpExtInst %float %1 FAbs %8967
%14855 = OpFMul %float %24093 %15305
%7151 = OpCompositeInsert %v4float %14855 %14002 3
OpStore %5482 %7151
OpReturn
OpFunctionEnd
