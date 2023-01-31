; SPIR-V
; Version: 1.0
; Generator: Google spiregg; 0
; Bound: 25272
; Schema: 0
OpCapability Shader
OpCapability ClipDistance
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Vertex %5061 "MainVs" %gl_ClipDistance %5759 %5760 %5761 %5936 %5762 %5763 %5764 %5765 %5766 %5767 %5937 %5768 %3981 %3486 %5188 %3488 %3489 %3490 %gl_Position
OpDecorate %gl_ClipDistance BuiltIn ClipDistance
OpDecorate %gl_Position BuiltIn Position
OpDecorate %5759 Location 0
OpDecorate %5760 Location 1
OpDecorate %5761 Location 2
OpDecorate %5936 Location 3
OpDecorate %5762 Location 4
OpDecorate %5763 Location 5
OpDecorate %5764 Location 6
OpDecorate %5765 Location 7
OpDecorate %5766 Location 8
OpDecorate %5767 Location 9
OpDecorate %5937 Location 10
OpDecorate %5768 Location 11
OpDecorate %3981 Location 12
OpDecorate %3486 Location 0
OpDecorate %5188 Location 1
OpDecorate %3488 Location 2
OpDecorate %3489 Location 3
OpDecorate %3490 Location 4
OpDecorate %4459 DescriptorSet 3
OpDecorate %4459 Binding 1
OpDecorate %3805 DescriptorSet 4
OpDecorate %3805 Binding 1
OpDecorate %3553 DescriptorSet 4
OpDecorate %3553 Binding 0
OpDecorate %_arr_mat4v4float_uint_2 ArrayStride 64
OpDecorate %_arr_v4float_uint_2 ArrayStride 16
OpMemberDecorate %_struct_425 0 Offset 272
OpMemberDecorate %_struct_425 1 Offset 304
OpMemberDecorate %_struct_425 2 Offset 320
OpMemberDecorate %_struct_425 3 Offset 512
OpMemberDecorate %_struct_425 3 MatrixStride 16
OpMemberDecorate %_struct_425 3 RowMajor
OpMemberDecorate %_struct_425 4 Offset 720
OpDecorate %_struct_425 Block
OpMemberDecorate %_struct_1020 0 Offset 44
OpMemberDecorate %_struct_1020 1 Offset 64
OpMemberDecorate %_struct_1020 2 Offset 68
OpDecorate %_struct_1020 Block
OpMemberDecorate %_struct_1209 0 Offset 16
OpMemberDecorate %_struct_1209 1 Offset 32
OpMemberDecorate %_struct_1209 2 Offset 48
OpMemberDecorate %_struct_1209 3 Offset 304
OpDecorate %_struct_1209 Block
OpMemberDecorate %_struct_1479 0 Offset 0
OpMemberDecorate %_struct_1479 1 Offset 16
OpMemberDecorate %_struct_1479 2 Offset 32
OpMemberDecorate %_struct_1479 3 Offset 64
OpMemberDecorate %_struct_1479 4 Offset 80
OpMemberDecorate %_struct_1479 5 Offset 256
OpDecorate %_struct_1479 Block
%float = OpTypeFloat 32
%float_0_5 = OpConstant %float 0.5
%float_0 = OpConstant %float 0
%float_1 = OpConstant %float 1
%uint = OpTypeInt 32 0
%uint_0 = OpConstant %uint 0
%int = OpTypeInt 32 1
%int_2 = OpConstant %int 2
%int_1 = OpConstant %int 1
%float_2 = OpConstant %float 2
%v2float = OpTypeVector %float 2
%768 = OpConstantComposite %v2float %float_1 %float_1
%int_0 = OpConstant %int 0
%float_n0_5 = OpConstant %float -0.5
%float_n1 = OpConstant %float -1
%v4float = OpTypeVector %float 4
%600 = OpConstantComposite %v4float %float_n1 %float_n1 %float_1 %float_0
%int_4 = OpConstant %int 4
%int_8 = OpConstant %int 8
%int_3 = OpConstant %int 3
%int_31 = OpConstant %int 31
%int_65535 = OpConstant %int 65535
%int_15 = OpConstant %int 15
%int_7 = OpConstant %int 7
%int_12 = OpConstant %int 12
%int_16 = OpConstant %int 16
%v3float = OpTypeVector %float 3
%626 = OpConstantComposite %v3float %float_0 %float_0 %float_1
%uint_3 = OpConstant %uint 3
%bool = OpTypeBool
%false = OpConstantFalse %bool
%float_0_00100000005 = OpConstant %float 0.00100000005
%uint_2 = OpConstant %uint 2
%1566 = OpConstantComposite %v2float %float_0_5 %float_0_5
%1823 = OpConstantComposite %v2float %float_0 %float_0
%uint_1 = OpConstant %uint 1
%float_238_839996 = OpConstant %float 238.839996
%float_84_7455978 = OpConstant %float 84.7455978
%float_0_0521326996 = OpConstant %float 0.0521326996
%799 = OpConstantComposite %v3float %float_0_0521326996 %float_0_0521326996 %float_0_0521326996
%float_2_4000001 = OpConstant %float 2.4000001
%2947 = OpConstantComposite %v3float %float_2_4000001 %float_2_4000001 %float_2_4000001
%float_0_0404499993 = OpConstant %float 0.0404499993
%float_3 = OpConstant %float 3
%float_n3 = OpConstant %float -3
%float_n5 = OpConstant %float -5
%float_4 = OpConstant %float 4
%2938 = OpConstantComposite %v4float %float_0 %float_0 %float_0 %float_0
%mat4v4float = OpTypeMatrix %v4float 4
%_arr_mat4v4float_uint_2 = OpTypeArray %mat4v4float %uint_2
%_arr_v4float_uint_2 = OpTypeArray %v4float %uint_2
%_struct_425 = OpTypeStruct %v4float %v3float %v3float %_arr_mat4v4float_uint_2 %_arr_v4float_uint_2
%_ptr_Uniform__struct_425 = OpTypePointer Uniform %_struct_425
%_struct_1020 = OpTypeStruct %float %float %float
%_ptr_PushConstant__struct_1020 = OpTypePointer PushConstant %_struct_1020
%v2uint = OpTypeVector %uint 2
%_struct_1209 = OpTypeStruct %v4float %v4float %v4float %v2uint
%_ptr_Uniform__struct_1209 = OpTypePointer Uniform %_struct_1209
%v3uint = OpTypeVector %uint 3
%_struct_1479 = OpTypeStruct %v4float %v4float %v4float %v4float %v4float %v3uint
%_ptr_Uniform__struct_1479 = OpTypePointer Uniform %_struct_1479
%_arr_float_uint_1 = OpTypeArray %float %uint_1
%_ptr_Output__arr_float_uint_1 = OpTypePointer Output %_arr_float_uint_1
%_ptr_Input_v2float = OpTypePointer Input %v2float
%_ptr_Input_v4float = OpTypePointer Input %v4float
%_ptr_Input_v3float = OpTypePointer Input %v3float
%_ptr_Output_v4float = OpTypePointer Output %v4float
%_ptr_Output_v3float = OpTypePointer Output %v3float
%void = OpTypeVoid
%1282 = OpTypeFunction %void
%_ptr_Output_float = OpTypePointer Output %float
%_ptr_Uniform_v4float = OpTypePointer Uniform %v4float
%_ptr_Uniform_float = OpTypePointer Uniform %float
%_ptr_Uniform_v3float = OpTypePointer Uniform %v3float
%_ptr_PushConstant_float = OpTypePointer PushConstant %float
%_ptr_Uniform_uint = OpTypePointer Uniform %uint
%_ptr_Uniform_mat4v4float = OpTypePointer Uniform %mat4v4float
%4459 = OpVariable %_ptr_Uniform__struct_425 Uniform
%6012 = OpVariable %_ptr_PushConstant__struct_1020 PushConstant
%3805 = OpVariable %_ptr_Uniform__struct_1209 Uniform
%3553 = OpVariable %_ptr_Uniform__struct_1479 Uniform
%gl_ClipDistance = OpVariable %_ptr_Output__arr_float_uint_1 Output
%5759 = OpVariable %_ptr_Input_v2float Input
%5760 = OpVariable %_ptr_Input_v4float Input
%5761 = OpVariable %_ptr_Input_v4float Input
%5936 = OpVariable %_ptr_Input_v4float Input
%5762 = OpVariable %_ptr_Input_v4float Input
%5763 = OpVariable %_ptr_Input_v4float Input
%5764 = OpVariable %_ptr_Input_v4float Input
%5765 = OpVariable %_ptr_Input_v4float Input
%5766 = OpVariable %_ptr_Input_v4float Input
%5767 = OpVariable %_ptr_Input_v4float Input
%5937 = OpVariable %_ptr_Input_v4float Input
%5768 = OpVariable %_ptr_Input_v3float Input
%3981 = OpVariable %_ptr_Input_v3float Input
%3486 = OpVariable %_ptr_Output_v4float Output
%5188 = OpVariable %_ptr_Output_v4float Output
%3488 = OpVariable %_ptr_Output_v4float Output
%3489 = OpVariable %_ptr_Output_v3float Output
%3490 = OpVariable %_ptr_Output_v4float Output
%gl_Position = OpVariable %_ptr_Output_v4float Output
%true = OpConstantTrue %bool
%13357 = OpUndef %v4float
%2 = OpUndef %float
%float_5657_41113 = OpConstant %float 5657.41113
%float_1231_67041 = OpConstant %float 1231.67041
%float_1542_87061 = OpConstant %float 1542.87061
%float_4574_43115 = OpConstant %float 4574.43115
%float_2969_86182 = OpConstant %float 2969.86182
%float_1944_34521 = OpConstant %float 1944.34521
%float_2704_22534 = OpConstant %float 2704.22534
%212 = OpConstantNull %v4float
%float_1718_43616 = OpConstant %float 1718.43616
%float_1271_89014 = OpConstant %float 1271.89014
%float_0_0773993805 = OpConstant %float 0.0773993805
%418 = OpConstantComposite %v3float %float_0_0773993805 %float_0_0773993805 %float_0_0773993805
%float_0_947867334 = OpConstant %float 0.947867334
%1533 = OpConstantComposite %v3float %float_0_947867334 %float_0_947867334 %float_0_947867334
%float_n2_5 = OpConstant %float -2.5
%float_1_5 = OpConstant %float 1.5
%float_n1_5 = OpConstant %float -1.5
%16427 = OpUndef %v2float
%10030 = OpUndef %v4float
%uint_4 = OpConstant %uint 4
%uint_5 = OpConstant %uint 5
%5061 = OpFunction %void None %1282
%11846 = OpLabel
%22883 = OpLoad %v2float %5759
%19276 = OpLoad %v4float %5760
%13443 = OpLoad %v4float %5761
%13444 = OpLoad %v4float %5936
%13445 = OpLoad %v4float %5762
%13446 = OpLoad %v4float %5763
%13447 = OpLoad %v4float %5764
%13448 = OpLoad %v4float %5765
%13449 = OpLoad %v4float %5766
%13450 = OpLoad %v4float %5767
%13451 = OpLoad %v4float %5937
%13519 = OpLoad %v3float %5768
%12751 = OpLoad %v3float %3981
%19155 = OpAccessChain %_ptr_Uniform_v3float %4459 %uint_1
%23486 = OpLoad %v3float %19155
%21694 = OpVectorShuffle %v2float %13450 %13450 2 3
%7928 = OpVectorTimesScalar %v2float %21694 %float_2
%17763 = OpFSub %v2float %7928 %768
%16782 = OpFMul %v2float %22883 %17763
%13588 = OpCompositeExtract %float %16782 0
%7293 = OpFMul %float %13588 %float_0_5
%23704 = OpCompositeExtract %float %13450 0
%14117 = OpFSub %float %float_1 %23704
%20352 = OpExtInst %float %1 Fma %7293 %14117 %float_0_5
%24390 = OpCompositeConstruct %v2float %20352 %20352
%18735 = OpCompositeExtract %float %16782 1
%19133 = OpFMul %float %18735 %float_n0_5
%23705 = OpCompositeExtract %float %13450 1
%14118 = OpFSub %float %float_1 %23705
%20353 = OpExtInst %float %1 Fma %19133 %14118 %float_0_5
%23402 = OpCompositeConstruct %v2float %20353 %20353
%8660 = OpCompositeExtract %float %13443 2
%23396 = OpCompositeExtract %float %13449 0
%12377 = OpCompositeExtract %float %22883 1
%12755 = OpExtInst %float %1 Fma %12377 %float_0_5 %float_0_5
%24126 = OpExtInst %float %1 FMix %8660 %23396 %12755
%23102 = OpVectorShuffle %v2float %23402 %212 0 1
%9406 = OpAccessChain %_ptr_PushConstant_float %6012 %uint_1
%14976 = OpLoad %float %9406
%24526 = OpFAdd %float %24126 %14976
%20820 = OpAccessChain %_ptr_PushConstant_float %6012 %uint_2
%22609 = OpLoad %float %20820
%8306 = OpFMul %float %24526 %22609
%21909 = OpCompositeConstruct %v2float %8306 %8306
%14098 = OpAccessChain %_ptr_PushConstant_float %6012 %uint_0
%7456 = OpLoad %float %14098
%8487 = OpCompositeConstruct %v2float %7456 %7456
%23611 = OpExtInst %v2float %1 FMix %23102 %21909 %8487
%20277 = OpVectorShuffle %v4float %24390 %23611 0 2 1 3
%8190 = OpAccessChain %_ptr_Uniform_float %3553 %uint_4 %int_0
%6901 = OpLoad %float %8190
%22400 = OpAccessChain %_ptr_Uniform_float %3553 %uint_4 %int_1
%21850 = OpLoad %float %22400
%6421 = OpCompositeConstruct %v2float %6901 %21850
%7401 = OpAccessChain %_ptr_Uniform_float %3553 %uint_4 %int_2
%10936 = OpLoad %float %7401
%22401 = OpAccessChain %_ptr_Uniform_float %3553 %uint_4 %int_3
%21851 = OpLoad %float %22401
%6422 = OpCompositeConstruct %v2float %10936 %21851
%7402 = OpAccessChain %_ptr_Uniform_float %3553 %uint_3 %int_3
%10937 = OpLoad %float %7402
%22970 = OpAccessChain %_ptr_Uniform_float %3553 %uint_3 %int_0
%19206 = OpLoad %float %22970
%6989 = OpConvertFToS %int %19206
%12179 = OpBitwiseAnd %int %6989 %int_31
%16174 = OpShiftRightArithmetic %int %12179 %int_4
%15642 = OpBitwiseAnd %int %16174 %int_1
%15588 = OpConvertSToF %float %15642
%14645 = OpAccessChain %_ptr_Uniform_uint %3553 %uint_5 %int_0
%24824 = OpLoad %uint %14645
%6713 = OpBitcast %int %24824
%8393 = OpBitwiseAnd %int %6713 %int_65535
%21255 = OpShiftRightArithmetic %int %8393 %int_0
%15939 = OpBitwiseAnd %int %21255 %int_15
%20591 = OpBitcast %uint %15939
%19958 = OpBitwiseAnd %int %12179 %int_15
%14671 = OpBitcast %uint %19958
%10842 = OpAccessChain %_ptr_Uniform_float %3553 %uint_3 %int_1
%8974 = OpLoad %float %10842
%24711 = OpConvertFToS %int %8974
%24386 = OpBitwiseAnd %int %24711 %int_15
%15497 = OpBitcast %uint %24386
%21805 = OpShiftRightArithmetic %int %8393 %int_4
%22874 = OpBitwiseAnd %int %21805 %int_15
%22708 = OpBitcast %uint %22874
%21806 = OpShiftRightArithmetic %int %8393 %int_8
%22875 = OpBitwiseAnd %int %21806 %int_15
%22709 = OpBitcast %uint %22875
%21807 = OpShiftRightArithmetic %int %8393 %int_12
%20404 = OpBitwiseAnd %int %21807 %int_15
%23505 = OpBitcast %uint %20404
%10843 = OpAccessChain %_ptr_Uniform_float %3553 %uint_3 %int_2
%8898 = OpLoad %float %10843
%7757 = OpConvertFToS %int %8898
%23763 = OpShiftRightArithmetic %int %7757 %int_12
%22027 = OpBitwiseAnd %int %23763 %int_15
%23506 = OpBitcast %uint %22027
%11108 = OpAccessChain %_ptr_Uniform_uint %3805 %uint_3 %int_1
%6476 = OpLoad %uint %11108
%7481 = OpBitcast %int %6476
%19977 = OpShiftRightArithmetic %int %7481 %int_16
%23862 = OpBitwiseAnd %int %19977 %int_15
%14700 = OpBitcast %uint %23862
OpSelectionMerge %11044 None
OpSwitch %uint_0 %11824
%11824 = OpLabel
%18701 = OpIEqual %bool %14700 %uint_2
OpSelectionMerge %15703 None
OpBranchConditional %18701 %17144 %24356
%24356 = OpLabel
OpSelectionMerge %20344 None
OpSwitch %uint_0 %17650
%17650 = OpLabel
OpSelectionMerge %7154 None
OpSwitch %20591 %7154 1 %13071 2 %6608 3 %10109 4 %10108 5 %10107 6 %10106 7 %10105 8 %10104 9 %13005 10 %17553 11 %13147 12 %12159 13 %25055
%25055 = OpLabel
%6705 = OpCompositeExtract %float %19276 0
%8088 = OpFMul %float %6705 %float_5657_41113
%21530 = OpExtInst %float %1 Fract %8088
%20499 = OpFSub %float %21530 %float_0_5
%10164 = OpFMul %float %20499 %float_2
%24980 = OpCompositeExtract %float %19276 2
%15725 = OpExtInst %float %1 FSign %10164
%18631 = OpConvertFToS %int %15725
%25195 = OpConvertSToF %float %18631
%13271 = OpExtInst %float %1 Fma %10164 %24980 %25195
OpBranch %7154
%12159 = OpLabel
%16377 = OpCompositeExtract %float %19276 2
%21012 = OpCompositeExtract %float %19276 0
%22331 = OpFMul %float %21012 %float_5657_41113
%23791 = OpExtInst %float %1 Fract %22331
%24357 = OpExtInst %float %1 Fma %16377 %23791 %float_1
OpBranch %7154
%13147 = OpLabel
%24163 = OpCompositeExtract %float %19276 0
%8089 = OpFMul %float %24163 %float_5657_41113
%21531 = OpExtInst %float %1 Fract %8089
%23691 = OpFSub %float %21531 %float_0_5
%23720 = OpFMul %float %23691 %float_2
OpBranch %7154
%17553 = OpLabel
%24164 = OpCompositeExtract %float %19276 0
%10330 = OpFMul %float %24164 %float_5657_41113
%22372 = OpExtInst %float %1 Fract %10330
OpBranch %7154
%13005 = OpLabel
%8569 = OpCompositeExtract %float %13449 3
OpBranch %7154
%10104 = OpLabel
%8570 = OpCompositeExtract %float %13449 2
OpBranch %7154
%10105 = OpLabel
%8571 = OpCompositeExtract %float %13449 1
OpBranch %7154
%10106 = OpLabel
%8572 = OpCompositeExtract %float %13446 3
OpBranch %7154
%10107 = OpLabel
%8573 = OpCompositeExtract %float %13444 3
OpBranch %7154
%10108 = OpLabel
%8574 = OpCompositeExtract %float %19276 3
OpBranch %7154
%10109 = OpLabel
%8575 = OpCompositeExtract %float %19276 1
OpBranch %7154
%6608 = OpLabel
%17777 = OpCompositeExtract %float %19276 2
%12028 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19796 = OpLoad %float %12028
%13228 = OpExtInst %float %1 Fma %17777 %19796 %float_1
OpBranch %7154
%13071 = OpLabel
%10277 = OpCompositeExtract %float %19276 2
%25259 = OpFAdd %float %10277 %float_1
OpBranch %7154
%7154 = OpLabel
%13174 = OpPhi %float %2 %17650 %13271 %25055 %24357 %12159 %23720 %13147 %22372 %17553 %8569 %13005 %8570 %10104 %8571 %10105 %8572 %10106 %8573 %10107 %8574 %10108 %8575 %10109 %13228 %6608 %25259 %13071
%16054 = OpPhi %bool %false %17650 %true %25055 %true %12159 %true %13147 %true %17553 %true %13005 %true %10104 %true %10105 %true %10106 %true %10107 %true %10108 %true %10109 %true %6608 %true %13071
OpSelectionMerge %23953 None
OpBranchConditional %16054 %20344 %23953
%23953 = OpLabel
OpBranch %20344
%20344 = OpLabel
%12960 = OpPhi %float %13174 %7154 %float_1 %23953
%19230 = OpFDiv %float %float_1 %12960
OpSelectionMerge %22453 None
OpSwitch %uint_0 %17651
%17651 = OpLabel
OpSelectionMerge %7155 None
OpSwitch %14671 %7155 1 %13072 2 %6609 3 %10115 4 %10114 5 %10113 6 %10112 7 %10111 8 %10110 9 %13006 10 %17554 11 %13148 12 %12160 13 %25056
%25056 = OpLabel
%6706 = OpCompositeExtract %float %19276 0
%8090 = OpFMul %float %6706 %float_1231_67041
%21532 = OpExtInst %float %1 Fract %8090
%20500 = OpFSub %float %21532 %float_0_5
%10165 = OpFMul %float %20500 %float_2
%24981 = OpCompositeExtract %float %19276 2
%15726 = OpExtInst %float %1 FSign %10165
%18632 = OpConvertFToS %int %15726
%25196 = OpConvertSToF %float %18632
%13272 = OpExtInst %float %1 Fma %10165 %24981 %25196
OpBranch %7155
%12160 = OpLabel
%16378 = OpCompositeExtract %float %19276 2
%21013 = OpCompositeExtract %float %19276 0
%22332 = OpFMul %float %21013 %float_1231_67041
%23792 = OpExtInst %float %1 Fract %22332
%24358 = OpExtInst %float %1 Fma %16378 %23792 %float_1
OpBranch %7155
%13148 = OpLabel
%24165 = OpCompositeExtract %float %19276 0
%8091 = OpFMul %float %24165 %float_1231_67041
%21533 = OpExtInst %float %1 Fract %8091
%23692 = OpFSub %float %21533 %float_0_5
%23721 = OpFMul %float %23692 %float_2
OpBranch %7155
%17554 = OpLabel
%24166 = OpCompositeExtract %float %19276 0
%10331 = OpFMul %float %24166 %float_1231_67041
%22373 = OpExtInst %float %1 Fract %10331
OpBranch %7155
%13006 = OpLabel
%8576 = OpCompositeExtract %float %13449 3
OpBranch %7155
%10110 = OpLabel
%8577 = OpCompositeExtract %float %13449 2
OpBranch %7155
%10111 = OpLabel
%8578 = OpCompositeExtract %float %13449 1
OpBranch %7155
%10112 = OpLabel
%8579 = OpCompositeExtract %float %13446 3
OpBranch %7155
%10113 = OpLabel
%8580 = OpCompositeExtract %float %13444 3
OpBranch %7155
%10114 = OpLabel
%8581 = OpCompositeExtract %float %19276 3
OpBranch %7155
%10115 = OpLabel
%8582 = OpCompositeExtract %float %19276 1
OpBranch %7155
%6609 = OpLabel
%17778 = OpCompositeExtract %float %19276 2
%12029 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19797 = OpLoad %float %12029
%13229 = OpExtInst %float %1 Fma %17778 %19797 %float_1
OpBranch %7155
%13072 = OpLabel
%10278 = OpCompositeExtract %float %19276 2
%25260 = OpFAdd %float %10278 %float_1
OpBranch %7155
%7155 = OpLabel
%13175 = OpPhi %float %2 %17651 %13272 %25056 %24358 %12160 %23721 %13148 %22373 %17554 %8576 %13006 %8577 %10110 %8578 %10111 %8579 %10112 %8580 %10113 %8581 %10114 %8582 %10115 %13229 %6609 %25260 %13072
%16055 = OpPhi %bool %false %17651 %true %25056 %true %12160 %true %13148 %true %17554 %true %13006 %true %10110 %true %10111 %true %10112 %true %10113 %true %10114 %true %10115 %true %6609 %true %13072
OpSelectionMerge %23954 None
OpBranchConditional %16055 %22453 %23954
%23954 = OpLabel
OpBranch %22453
%22453 = OpLabel
%12904 = OpPhi %float %13175 %7155 %float_1 %23954
OpSelectionMerge %19280 None
OpSwitch %uint_0 %17652
%17652 = OpLabel
OpSelectionMerge %7156 None
OpSwitch %15497 %7156 1 %13073 2 %6610 3 %10121 4 %10120 5 %10119 6 %10118 7 %10117 8 %10116 9 %13007 10 %17555 11 %13149 12 %12161 13 %25057
%25057 = OpLabel
%6707 = OpCompositeExtract %float %19276 0
%8092 = OpFMul %float %6707 %float_1542_87061
%21534 = OpExtInst %float %1 Fract %8092
%20501 = OpFSub %float %21534 %float_0_5
%10166 = OpFMul %float %20501 %float_2
%24982 = OpCompositeExtract %float %19276 2
%15727 = OpExtInst %float %1 FSign %10166
%18633 = OpConvertFToS %int %15727
%25197 = OpConvertSToF %float %18633
%13273 = OpExtInst %float %1 Fma %10166 %24982 %25197
OpBranch %7156
%12161 = OpLabel
%16379 = OpCompositeExtract %float %19276 2
%21014 = OpCompositeExtract %float %19276 0
%22333 = OpFMul %float %21014 %float_1542_87061
%23793 = OpExtInst %float %1 Fract %22333
%24359 = OpExtInst %float %1 Fma %16379 %23793 %float_1
OpBranch %7156
%13149 = OpLabel
%24167 = OpCompositeExtract %float %19276 0
%8093 = OpFMul %float %24167 %float_1542_87061
%21535 = OpExtInst %float %1 Fract %8093
%23693 = OpFSub %float %21535 %float_0_5
%23722 = OpFMul %float %23693 %float_2
OpBranch %7156
%17555 = OpLabel
%24168 = OpCompositeExtract %float %19276 0
%10332 = OpFMul %float %24168 %float_1542_87061
%22374 = OpExtInst %float %1 Fract %10332
OpBranch %7156
%13007 = OpLabel
%8583 = OpCompositeExtract %float %13449 3
OpBranch %7156
%10116 = OpLabel
%8584 = OpCompositeExtract %float %13449 2
OpBranch %7156
%10117 = OpLabel
%8585 = OpCompositeExtract %float %13449 1
OpBranch %7156
%10118 = OpLabel
%8586 = OpCompositeExtract %float %13446 3
OpBranch %7156
%10119 = OpLabel
%8587 = OpCompositeExtract %float %13444 3
OpBranch %7156
%10120 = OpLabel
%8588 = OpCompositeExtract %float %19276 3
OpBranch %7156
%10121 = OpLabel
%8589 = OpCompositeExtract %float %19276 1
OpBranch %7156
%6610 = OpLabel
%17779 = OpCompositeExtract %float %19276 2
%12030 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19798 = OpLoad %float %12030
%13230 = OpExtInst %float %1 Fma %17779 %19798 %float_1
OpBranch %7156
%13073 = OpLabel
%10279 = OpCompositeExtract %float %19276 2
%25261 = OpFAdd %float %10279 %float_1
OpBranch %7156
%7156 = OpLabel
%13176 = OpPhi %float %2 %17652 %13273 %25057 %24359 %12161 %23722 %13149 %22374 %17555 %8583 %13007 %8584 %10116 %8585 %10117 %8586 %10118 %8587 %10119 %8588 %10120 %8589 %10121 %13230 %6610 %25261 %13073
%16056 = OpPhi %bool %false %17652 %true %25057 %true %12161 %true %13149 %true %17555 %true %13007 %true %10116 %true %10117 %true %10118 %true %10119 %true %10120 %true %10121 %true %6610 %true %13073
OpSelectionMerge %23955 None
OpBranchConditional %16056 %19280 %23955
%23955 = OpLabel
OpBranch %19280
%19280 = OpLabel
%22648 = OpPhi %float %13176 %7156 %float_1 %23955
%7720 = OpCompositeConstruct %v2float %12904 %22648
OpSelectionMerge %19261 None
OpSwitch %uint_0 %17653
%17653 = OpLabel
OpSelectionMerge %7157 None
OpSwitch %22708 %7157 1 %13074 2 %6611 3 %10127 4 %10126 5 %10125 6 %10124 7 %10123 8 %10122 9 %13008 10 %17556 11 %13150 12 %12162 13 %25058
%25058 = OpLabel
%6708 = OpCompositeExtract %float %19276 0
%8094 = OpFMul %float %6708 %float_2969_86182
%21536 = OpExtInst %float %1 Fract %8094
%20502 = OpFSub %float %21536 %float_0_5
%10167 = OpFMul %float %20502 %float_2
%24983 = OpCompositeExtract %float %19276 2
%15728 = OpExtInst %float %1 FSign %10167
%18634 = OpConvertFToS %int %15728
%25198 = OpConvertSToF %float %18634
%13274 = OpExtInst %float %1 Fma %10167 %24983 %25198
OpBranch %7157
%12162 = OpLabel
%16380 = OpCompositeExtract %float %19276 2
%21015 = OpCompositeExtract %float %19276 0
%22334 = OpFMul %float %21015 %float_2969_86182
%23794 = OpExtInst %float %1 Fract %22334
%24360 = OpExtInst %float %1 Fma %16380 %23794 %float_1
OpBranch %7157
%13150 = OpLabel
%24169 = OpCompositeExtract %float %19276 0
%8095 = OpFMul %float %24169 %float_2969_86182
%21537 = OpExtInst %float %1 Fract %8095
%23694 = OpFSub %float %21537 %float_0_5
%23723 = OpFMul %float %23694 %float_2
OpBranch %7157
%17556 = OpLabel
%24170 = OpCompositeExtract %float %19276 0
%10333 = OpFMul %float %24170 %float_2969_86182
%22375 = OpExtInst %float %1 Fract %10333
OpBranch %7157
%13008 = OpLabel
%8590 = OpCompositeExtract %float %13449 3
OpBranch %7157
%10122 = OpLabel
%8591 = OpCompositeExtract %float %13449 2
OpBranch %7157
%10123 = OpLabel
%8592 = OpCompositeExtract %float %13449 1
OpBranch %7157
%10124 = OpLabel
%8593 = OpCompositeExtract %float %13446 3
OpBranch %7157
%10125 = OpLabel
%8594 = OpCompositeExtract %float %13444 3
OpBranch %7157
%10126 = OpLabel
%8595 = OpCompositeExtract %float %19276 3
OpBranch %7157
%10127 = OpLabel
%8596 = OpCompositeExtract %float %19276 1
OpBranch %7157
%6611 = OpLabel
%17780 = OpCompositeExtract %float %19276 2
%12031 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19799 = OpLoad %float %12031
%13231 = OpExtInst %float %1 Fma %17780 %19799 %float_1
OpBranch %7157
%13074 = OpLabel
%10280 = OpCompositeExtract %float %19276 2
%25262 = OpFAdd %float %10280 %float_1
OpBranch %7157
%7157 = OpLabel
%13177 = OpPhi %float %2 %17653 %13274 %25058 %24360 %12162 %23723 %13150 %22375 %17556 %8590 %13008 %8591 %10122 %8592 %10123 %8593 %10124 %8594 %10125 %8595 %10126 %8596 %10127 %13231 %6611 %25262 %13074
%16057 = OpPhi %bool %false %17653 %true %25058 %true %12162 %true %13150 %true %17556 %true %13008 %true %10122 %true %10123 %true %10124 %true %10125 %true %10126 %true %10127 %true %6611 %true %13074
OpSelectionMerge %23956 None
OpBranchConditional %16057 %19261 %23956
%23956 = OpLabel
OpBranch %19261
%19261 = OpLabel
%20617 = OpPhi %float %13177 %7157 %float_1 %23956
%14668 = OpVectorShuffle %v2float %20277 %20277 0 1
%13164 = OpFSub %v2float %14668 %1566
%19474 = OpFMul %float %10937 %20617
%9168 = OpExtInst %float %1 Cos %19474
%24025 = OpExtInst %float %1 Sin %19474
%21312 = OpCompositeExtract %float %13164 0
%22494 = OpCompositeExtract %float %13164 1
%16520 = OpFNegate %float %24025
%7447 = OpFMul %float %22494 %16520
%6239 = OpExtInst %float %1 Fma %21312 %9168 %7447
%10986 = OpCompositeInsert %v4float %6239 %10030 0
%24799 = OpFMul %float %22494 %9168
%17048 = OpExtInst %float %1 Fma %21312 %24025 %24799
%22634 = OpCompositeInsert %v4float %17048 %10986 1
%16192 = OpVectorShuffle %v2float %22634 %22634 0 1
%22043 = OpFDiv %v2float %16192 %6422
%10977 = OpVectorTimesScalar %v2float %22043 %19230
%22113 = OpExtInst %v2float %1 Fma %6421 %7720 %1566
%20739 = OpExtInst %v2float %1 Fract %22113
%23392 = OpFAdd %v2float %10977 %20739
%10433 = OpCompositeExtract %float %19276 0
%13173 = OpFMul %float %10433 %float_1718_43616
%6942 = OpFMul %float %10433 %float_1271_89014
%21131 = OpCompositeConstruct %v2float %13173 %6942
%14008 = OpExtInst %v2float %1 Fract %21131
%13335 = OpCompositeConstruct %v2float %15588 %15588
%14887 = OpExtInst %v2float %1 FMix %1823 %14008 %13335
%12230 = OpFAdd %v2float %23392 %14887
%10219 = OpVectorShuffle %v4float %22634 %12230 4 5 2 3
OpSelectionMerge %19318 None
OpSwitch %uint_0 %17654
%17654 = OpLabel
OpSelectionMerge %7158 None
OpSwitch %22709 %7158 1 %13075 2 %6612 3 %10133 4 %10132 5 %10131 6 %10130 7 %10129 8 %10128 9 %13009 10 %6268 11 %20933 12 %13151 13 %13770
%13770 = OpLabel
%19309 = OpFMul %float %10433 %float_1944_34521
%11533 = OpExtInst %float %1 Fract %19309
%20503 = OpFSub %float %11533 %float_0_5
%10168 = OpFMul %float %20503 %float_2
%24984 = OpCompositeExtract %float %19276 2
%15729 = OpExtInst %float %1 FSign %10168
%18635 = OpConvertFToS %int %15729
%25199 = OpConvertSToF %float %18635
%13275 = OpExtInst %float %1 Fma %10168 %24984 %25199
OpBranch %7158
%13151 = OpLabel
%24171 = OpCompositeExtract %float %19276 2
%24948 = OpFMul %float %10433 %float_1944_34521
%23795 = OpExtInst %float %1 Fract %24948
%24361 = OpExtInst %float %1 Fma %24171 %23795 %float_1
OpBranch %7158
%20933 = OpLabel
%17696 = OpFMul %float %10433 %float_1944_34521
%11534 = OpExtInst %float %1 Fract %17696
%23695 = OpFSub %float %11534 %float_0_5
%23724 = OpFMul %float %23695 %float_2
OpBranch %7158
%6268 = OpLabel
%19938 = OpFMul %float %10433 %float_1944_34521
%12375 = OpExtInst %float %1 Fract %19938
OpBranch %7158
%13009 = OpLabel
%8597 = OpCompositeExtract %float %13449 3
OpBranch %7158
%10128 = OpLabel
%8598 = OpCompositeExtract %float %13449 2
OpBranch %7158
%10129 = OpLabel
%8599 = OpCompositeExtract %float %13449 1
OpBranch %7158
%10130 = OpLabel
%8600 = OpCompositeExtract %float %13446 3
OpBranch %7158
%10131 = OpLabel
%8601 = OpCompositeExtract %float %13444 3
OpBranch %7158
%10132 = OpLabel
%8602 = OpCompositeExtract %float %19276 3
OpBranch %7158
%10133 = OpLabel
%8603 = OpCompositeExtract %float %19276 1
OpBranch %7158
%6612 = OpLabel
%17781 = OpCompositeExtract %float %19276 2
%12032 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19800 = OpLoad %float %12032
%13232 = OpExtInst %float %1 Fma %17781 %19800 %float_1
OpBranch %7158
%13075 = OpLabel
%10281 = OpCompositeExtract %float %19276 2
%25263 = OpFAdd %float %10281 %float_1
OpBranch %7158
%7158 = OpLabel
%13178 = OpPhi %float %2 %17654 %13275 %13770 %24361 %13151 %23724 %20933 %12375 %6268 %8597 %13009 %8598 %10128 %8599 %10129 %8600 %10130 %8601 %10131 %8602 %10132 %8603 %10133 %13232 %6612 %25263 %13075
%16058 = OpPhi %bool %false %17654 %true %13770 %true %13151 %true %20933 %true %6268 %true %13009 %true %10128 %true %10129 %true %10130 %true %10131 %true %10132 %true %10133 %true %6612 %true %13075
OpSelectionMerge %23957 None
OpBranchConditional %16058 %19318 %23957
%23957 = OpLabel
OpBranch %19318
%19318 = OpLabel
%19167 = OpPhi %float %13178 %7158 %float_1 %23957
%18416 = OpCompositeInsert %v4float %19167 %10219 2
%23355 = OpCompositeInsert %v4float %float_1 %18416 3
%14019 = OpUGreaterThanEqual %bool %14700 %uint_3
OpSelectionMerge %17414 None
OpBranchConditional %14019 %24362 %17414
%24362 = OpLabel
OpSelectionMerge %19319 None
OpSwitch %uint_0 %17655
%17655 = OpLabel
OpSelectionMerge %7159 None
OpSwitch %23505 %7159 1 %13076 2 %6613 3 %10139 4 %10138 5 %10137 6 %10136 7 %10135 8 %10134 9 %13010 10 %6269 11 %20934 12 %13152 13 %13771
%13771 = OpLabel
%19310 = OpFMul %float %10433 %float_2704_22534
%11535 = OpExtInst %float %1 Fract %19310
%20504 = OpFSub %float %11535 %float_0_5
%10169 = OpFMul %float %20504 %float_2
%24985 = OpCompositeExtract %float %19276 2
%15730 = OpExtInst %float %1 FSign %10169
%18636 = OpConvertFToS %int %15730
%25200 = OpConvertSToF %float %18636
%13276 = OpExtInst %float %1 Fma %10169 %24985 %25200
OpBranch %7159
%13152 = OpLabel
%24172 = OpCompositeExtract %float %19276 2
%24949 = OpFMul %float %10433 %float_2704_22534
%23796 = OpExtInst %float %1 Fract %24949
%24363 = OpExtInst %float %1 Fma %24172 %23796 %float_1
OpBranch %7159
%20934 = OpLabel
%17697 = OpFMul %float %10433 %float_2704_22534
%11536 = OpExtInst %float %1 Fract %17697
%23696 = OpFSub %float %11536 %float_0_5
%23725 = OpFMul %float %23696 %float_2
OpBranch %7159
%6269 = OpLabel
%19939 = OpFMul %float %10433 %float_2704_22534
%12376 = OpExtInst %float %1 Fract %19939
OpBranch %7159
%13010 = OpLabel
%8604 = OpCompositeExtract %float %13449 3
OpBranch %7159
%10134 = OpLabel
%8605 = OpCompositeExtract %float %13449 2
OpBranch %7159
%10135 = OpLabel
%8606 = OpCompositeExtract %float %13449 1
OpBranch %7159
%10136 = OpLabel
%8607 = OpCompositeExtract %float %13446 3
OpBranch %7159
%10137 = OpLabel
%8608 = OpCompositeExtract %float %13444 3
OpBranch %7159
%10138 = OpLabel
%8609 = OpCompositeExtract %float %19276 3
OpBranch %7159
%10139 = OpLabel
%8610 = OpCompositeExtract %float %19276 1
OpBranch %7159
%6613 = OpLabel
%17782 = OpCompositeExtract %float %19276 2
%12033 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19801 = OpLoad %float %12033
%13233 = OpExtInst %float %1 Fma %17782 %19801 %float_1
OpBranch %7159
%13076 = OpLabel
%10282 = OpCompositeExtract %float %19276 2
%25264 = OpFAdd %float %10282 %float_1
OpBranch %7159
%7159 = OpLabel
%13179 = OpPhi %float %2 %17655 %13276 %13771 %24363 %13152 %23725 %20934 %12376 %6269 %8604 %13010 %8605 %10134 %8606 %10135 %8607 %10136 %8608 %10137 %8609 %10138 %8610 %10139 %13233 %6613 %25264 %13076
%16059 = OpPhi %bool %false %17655 %true %13771 %true %13152 %true %20934 %true %6269 %true %13010 %true %10134 %true %10135 %true %10136 %true %10137 %true %10138 %true %10139 %true %6613 %true %13076
OpSelectionMerge %23958 None
OpBranchConditional %16059 %19319 %23958
%23958 = OpLabel
OpBranch %19319
%19319 = OpLabel
%22340 = OpPhi %float %13179 %7159 %float_1 %23958
%10002 = OpCompositeInsert %v4float %22340 %23355 3
OpBranch %17414
%17414 = OpLabel
%12905 = OpPhi %v4float %23355 %19318 %10002 %19319
OpSelectionMerge %19299 None
OpSwitch %uint_0 %17656
%17656 = OpLabel
OpSelectionMerge %7160 None
OpSwitch %23506 %7160 1 %13077 2 %6614 3 %10145 4 %10144 5 %10143 6 %10142 7 %10141 8 %10140 9 %13011 10 %6270 11 %20935 12 %13153 13 %13772
%13772 = OpLabel
%19311 = OpFMul %float %10433 %float_4574_43115
%11537 = OpExtInst %float %1 Fract %19311
%20505 = OpFSub %float %11537 %float_0_5
%10170 = OpFMul %float %20505 %float_2
%24986 = OpCompositeExtract %float %19276 2
%15731 = OpExtInst %float %1 FSign %10170
%18637 = OpConvertFToS %int %15731
%25201 = OpConvertSToF %float %18637
%13277 = OpExtInst %float %1 Fma %10170 %24986 %25201
OpBranch %7160
%13153 = OpLabel
%24173 = OpCompositeExtract %float %19276 2
%24950 = OpFMul %float %10433 %float_4574_43115
%23797 = OpExtInst %float %1 Fract %24950
%24364 = OpExtInst %float %1 Fma %24173 %23797 %float_1
OpBranch %7160
%20935 = OpLabel
%17698 = OpFMul %float %10433 %float_4574_43115
%11538 = OpExtInst %float %1 Fract %17698
%23697 = OpFSub %float %11538 %float_0_5
%23726 = OpFMul %float %23697 %float_2
OpBranch %7160
%6270 = OpLabel
%19940 = OpFMul %float %10433 %float_4574_43115
%12378 = OpExtInst %float %1 Fract %19940
OpBranch %7160
%13011 = OpLabel
%8611 = OpCompositeExtract %float %13449 3
OpBranch %7160
%10140 = OpLabel
%8612 = OpCompositeExtract %float %13449 2
OpBranch %7160
%10141 = OpLabel
%8613 = OpCompositeExtract %float %13449 1
OpBranch %7160
%10142 = OpLabel
%8614 = OpCompositeExtract %float %13446 3
OpBranch %7160
%10143 = OpLabel
%8615 = OpCompositeExtract %float %13444 3
OpBranch %7160
%10144 = OpLabel
%8616 = OpCompositeExtract %float %19276 3
OpBranch %7160
%10145 = OpLabel
%8617 = OpCompositeExtract %float %19276 1
OpBranch %7160
%6614 = OpLabel
%17783 = OpCompositeExtract %float %19276 2
%12034 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19802 = OpLoad %float %12034
%13234 = OpExtInst %float %1 Fma %17783 %19802 %float_1
OpBranch %7160
%13077 = OpLabel
%10283 = OpCompositeExtract %float %19276 2
%25265 = OpFAdd %float %10283 %float_1
OpBranch %7160
%7160 = OpLabel
%13180 = OpPhi %float %2 %17656 %13277 %13772 %24364 %13153 %23726 %20935 %12378 %6270 %8611 %13011 %8612 %10140 %8613 %10141 %8614 %10142 %8615 %10143 %8616 %10144 %8617 %10145 %13234 %6614 %25265 %13077
%16060 = OpPhi %bool %false %17656 %true %13772 %true %13153 %true %20935 %true %6270 %true %13011 %true %10140 %true %10141 %true %10142 %true %10143 %true %10144 %true %10145 %true %6614 %true %13077
OpSelectionMerge %23959 None
OpBranchConditional %16060 %19299 %23959
%23959 = OpLabel
OpBranch %19299
%19299 = OpLabel
%20309 = OpPhi %float %13180 %7160 %float_1 %23959
%16038 = OpCompositeExtract %float %12905 3
%22206 = OpFMul %float %16038 %20309
%18602 = OpCompositeInsert %v4float %22206 %12905 3
OpBranch %11044
%17144 = OpLabel
OpSelectionMerge %19281 None
OpSwitch %uint_0 %17657
%17657 = OpLabel
OpSelectionMerge %7161 None
OpSwitch %20591 %7161 1 %13078 2 %6615 3 %10151 4 %10150 5 %10149 6 %10148 7 %10147 8 %10146 9 %13012 10 %17557 11 %13154 12 %12163 13 %25059
%25059 = OpLabel
%6709 = OpCompositeExtract %float %19276 0
%8096 = OpFMul %float %6709 %float_5657_41113
%21538 = OpExtInst %float %1 Fract %8096
%20506 = OpFSub %float %21538 %float_0_5
%10171 = OpFMul %float %20506 %float_2
%24987 = OpCompositeExtract %float %19276 2
%15732 = OpExtInst %float %1 FSign %10171
%18638 = OpConvertFToS %int %15732
%25202 = OpConvertSToF %float %18638
%13278 = OpExtInst %float %1 Fma %10171 %24987 %25202
OpBranch %7161
%12163 = OpLabel
%16381 = OpCompositeExtract %float %19276 2
%21016 = OpCompositeExtract %float %19276 0
%22335 = OpFMul %float %21016 %float_5657_41113
%23798 = OpExtInst %float %1 Fract %22335
%24365 = OpExtInst %float %1 Fma %16381 %23798 %float_1
OpBranch %7161
%13154 = OpLabel
%24174 = OpCompositeExtract %float %19276 0
%8097 = OpFMul %float %24174 %float_5657_41113
%21539 = OpExtInst %float %1 Fract %8097
%23698 = OpFSub %float %21539 %float_0_5
%23727 = OpFMul %float %23698 %float_2
OpBranch %7161
%17557 = OpLabel
%24175 = OpCompositeExtract %float %19276 0
%10334 = OpFMul %float %24175 %float_5657_41113
%22376 = OpExtInst %float %1 Fract %10334
OpBranch %7161
%13012 = OpLabel
%8618 = OpCompositeExtract %float %13449 3
OpBranch %7161
%10146 = OpLabel
%8619 = OpCompositeExtract %float %13449 2
OpBranch %7161
%10147 = OpLabel
%8620 = OpCompositeExtract %float %13449 1
OpBranch %7161
%10148 = OpLabel
%8621 = OpCompositeExtract %float %13446 3
OpBranch %7161
%10149 = OpLabel
%8622 = OpCompositeExtract %float %13444 3
OpBranch %7161
%10150 = OpLabel
%8623 = OpCompositeExtract %float %19276 3
OpBranch %7161
%10151 = OpLabel
%8624 = OpCompositeExtract %float %19276 1
OpBranch %7161
%6615 = OpLabel
%17784 = OpCompositeExtract %float %19276 2
%12035 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19803 = OpLoad %float %12035
%13235 = OpExtInst %float %1 Fma %17784 %19803 %float_1
OpBranch %7161
%13078 = OpLabel
%10284 = OpCompositeExtract %float %19276 2
%25266 = OpFAdd %float %10284 %float_1
OpBranch %7161
%7161 = OpLabel
%13181 = OpPhi %float %2 %17657 %13278 %25059 %24365 %12163 %23727 %13154 %22376 %17557 %8618 %13012 %8619 %10146 %8620 %10147 %8621 %10148 %8622 %10149 %8623 %10150 %8624 %10151 %13235 %6615 %25266 %13078
%16061 = OpPhi %bool %false %17657 %true %25059 %true %12163 %true %13154 %true %17557 %true %13012 %true %10146 %true %10147 %true %10148 %true %10149 %true %10150 %true %10151 %true %6615 %true %13078
OpSelectionMerge %23960 None
OpBranchConditional %16061 %19281 %23960
%23960 = OpLabel
OpBranch %19281
%19281 = OpLabel
%20482 = OpPhi %float %13181 %7161 %float_1 %23960
%25257 = OpCompositeConstruct %v2float %20482 %20482
%13079 = OpFMul %v2float %6422 %25257
%11181 = OpFDiv %v2float %768 %13079
OpSelectionMerge %22454 None
OpSwitch %uint_0 %17658
%17658 = OpLabel
OpSelectionMerge %7162 None
OpSwitch %14671 %7162 1 %13080 2 %6616 3 %10157 4 %10156 5 %10155 6 %10154 7 %10153 8 %10152 9 %13013 10 %17558 11 %13155 12 %12164 13 %25060
%25060 = OpLabel
%6710 = OpCompositeExtract %float %19276 0
%8098 = OpFMul %float %6710 %float_1231_67041
%21540 = OpExtInst %float %1 Fract %8098
%20507 = OpFSub %float %21540 %float_0_5
%10172 = OpFMul %float %20507 %float_2
%24988 = OpCompositeExtract %float %19276 2
%15733 = OpExtInst %float %1 FSign %10172
%18639 = OpConvertFToS %int %15733
%25203 = OpConvertSToF %float %18639
%13279 = OpExtInst %float %1 Fma %10172 %24988 %25203
OpBranch %7162
%12164 = OpLabel
%16382 = OpCompositeExtract %float %19276 2
%21017 = OpCompositeExtract %float %19276 0
%22336 = OpFMul %float %21017 %float_1231_67041
%23799 = OpExtInst %float %1 Fract %22336
%24366 = OpExtInst %float %1 Fma %16382 %23799 %float_1
OpBranch %7162
%13155 = OpLabel
%24176 = OpCompositeExtract %float %19276 0
%8099 = OpFMul %float %24176 %float_1231_67041
%21541 = OpExtInst %float %1 Fract %8099
%23699 = OpFSub %float %21541 %float_0_5
%23728 = OpFMul %float %23699 %float_2
OpBranch %7162
%17558 = OpLabel
%24177 = OpCompositeExtract %float %19276 0
%10335 = OpFMul %float %24177 %float_1231_67041
%22377 = OpExtInst %float %1 Fract %10335
OpBranch %7162
%13013 = OpLabel
%8625 = OpCompositeExtract %float %13449 3
OpBranch %7162
%10152 = OpLabel
%8626 = OpCompositeExtract %float %13449 2
OpBranch %7162
%10153 = OpLabel
%8627 = OpCompositeExtract %float %13449 1
OpBranch %7162
%10154 = OpLabel
%8628 = OpCompositeExtract %float %13446 3
OpBranch %7162
%10155 = OpLabel
%8629 = OpCompositeExtract %float %13444 3
OpBranch %7162
%10156 = OpLabel
%8630 = OpCompositeExtract %float %19276 3
OpBranch %7162
%10157 = OpLabel
%8631 = OpCompositeExtract %float %19276 1
OpBranch %7162
%6616 = OpLabel
%17785 = OpCompositeExtract %float %19276 2
%12036 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19804 = OpLoad %float %12036
%13236 = OpExtInst %float %1 Fma %17785 %19804 %float_1
OpBranch %7162
%13080 = OpLabel
%10285 = OpCompositeExtract %float %19276 2
%25267 = OpFAdd %float %10285 %float_1
OpBranch %7162
%7162 = OpLabel
%13182 = OpPhi %float %2 %17658 %13279 %25060 %24366 %12164 %23728 %13155 %22377 %17558 %8625 %13013 %8626 %10152 %8627 %10153 %8628 %10154 %8629 %10155 %8630 %10156 %8631 %10157 %13236 %6616 %25267 %13080
%16062 = OpPhi %bool %false %17658 %true %25060 %true %12164 %true %13155 %true %17558 %true %13013 %true %10152 %true %10153 %true %10154 %true %10155 %true %10156 %true %10157 %true %6616 %true %13080
OpSelectionMerge %23961 None
OpBranchConditional %16062 %22454 %23961
%23961 = OpLabel
OpBranch %22454
%22454 = OpLabel
%12906 = OpPhi %float %13182 %7162 %float_1 %23961
OpSelectionMerge %19282 None
OpSwitch %uint_0 %17659
%17659 = OpLabel
OpSelectionMerge %7163 None
OpSwitch %15497 %7163 1 %13081 2 %6617 3 %10163 4 %10162 5 %10161 6 %10160 7 %10159 8 %10158 9 %13014 10 %17559 11 %13156 12 %12165 13 %25061
%25061 = OpLabel
%6711 = OpCompositeExtract %float %19276 0
%8100 = OpFMul %float %6711 %float_1542_87061
%21542 = OpExtInst %float %1 Fract %8100
%20508 = OpFSub %float %21542 %float_0_5
%10173 = OpFMul %float %20508 %float_2
%24989 = OpCompositeExtract %float %19276 2
%15734 = OpExtInst %float %1 FSign %10173
%18640 = OpConvertFToS %int %15734
%25204 = OpConvertSToF %float %18640
%13280 = OpExtInst %float %1 Fma %10173 %24989 %25204
OpBranch %7163
%12165 = OpLabel
%16383 = OpCompositeExtract %float %19276 2
%21018 = OpCompositeExtract %float %19276 0
%22337 = OpFMul %float %21018 %float_1542_87061
%23800 = OpExtInst %float %1 Fract %22337
%24367 = OpExtInst %float %1 Fma %16383 %23800 %float_1
OpBranch %7163
%13156 = OpLabel
%24178 = OpCompositeExtract %float %19276 0
%8101 = OpFMul %float %24178 %float_1542_87061
%21543 = OpExtInst %float %1 Fract %8101
%23700 = OpFSub %float %21543 %float_0_5
%23729 = OpFMul %float %23700 %float_2
OpBranch %7163
%17559 = OpLabel
%24179 = OpCompositeExtract %float %19276 0
%10336 = OpFMul %float %24179 %float_1542_87061
%22378 = OpExtInst %float %1 Fract %10336
OpBranch %7163
%13014 = OpLabel
%8632 = OpCompositeExtract %float %13449 3
OpBranch %7163
%10158 = OpLabel
%8633 = OpCompositeExtract %float %13449 2
OpBranch %7163
%10159 = OpLabel
%8634 = OpCompositeExtract %float %13449 1
OpBranch %7163
%10160 = OpLabel
%8635 = OpCompositeExtract %float %13446 3
OpBranch %7163
%10161 = OpLabel
%8636 = OpCompositeExtract %float %13444 3
OpBranch %7163
%10162 = OpLabel
%8637 = OpCompositeExtract %float %19276 3
OpBranch %7163
%10163 = OpLabel
%8638 = OpCompositeExtract %float %19276 1
OpBranch %7163
%6617 = OpLabel
%17786 = OpCompositeExtract %float %19276 2
%12037 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19805 = OpLoad %float %12037
%13237 = OpExtInst %float %1 Fma %17786 %19805 %float_1
OpBranch %7163
%13081 = OpLabel
%10286 = OpCompositeExtract %float %19276 2
%25268 = OpFAdd %float %10286 %float_1
OpBranch %7163
%7163 = OpLabel
%13183 = OpPhi %float %2 %17659 %13280 %25061 %24367 %12165 %23729 %13156 %22378 %17559 %8632 %13014 %8633 %10158 %8634 %10159 %8635 %10160 %8636 %10161 %8637 %10162 %8638 %10163 %13237 %6617 %25268 %13081
%16063 = OpPhi %bool %false %17659 %true %25061 %true %12165 %true %13156 %true %17559 %true %13014 %true %10158 %true %10159 %true %10160 %true %10161 %true %10162 %true %10163 %true %6617 %true %13081
OpSelectionMerge %23962 None
OpBranchConditional %16063 %19282 %23962
%23962 = OpLabel
OpBranch %19282
%19282 = OpLabel
%18233 = OpPhi %float %13183 %7163 %float_1 %23962
%6494 = OpCompositeConstruct %v2float %12906 %18233
%9783 = OpExtInst %v2float %1 Fma %6421 %6494 %1566
%14034 = OpFSub %v2float %9783 %1566
%12281 = OpCompositeExtract %float %19276 0
%14786 = OpFMul %float %12281 %float_1718_43616
%6943 = OpFMul %float %12281 %float_1271_89014
%21132 = OpCompositeConstruct %v2float %14786 %6943
%14009 = OpExtInst %v2float %1 Fract %21132
%13336 = OpCompositeConstruct %v2float %15588 %15588
%14925 = OpExtInst %v2float %1 FMix %1823 %14009 %13336
%8730 = OpFAdd %v2float %14034 %14925
%20310 = OpCompositeExtract %float %11181 1
%19685 = OpCompositeExtract %float %8730 1
%15550 = OpFMul %float %19685 %20310
%6697 = OpCompositeInsert %v2float %15550 %8730 1
%9902 = OpVectorShuffle %v4float %20277 %6697 4 5 2 3
%10678 = OpCompositeExtract %float %11181 0
%12469 = OpCompositeInsert %v4float %10678 %9902 2
OpSelectionMerge %19320 None
OpSwitch %uint_0 %17660
%17660 = OpLabel
OpSelectionMerge %7164 None
OpSwitch %23506 %7164 1 %13082 2 %6618 3 %10180 4 %10179 5 %10178 6 %10177 7 %10176 8 %10175 9 %13015 10 %6271 11 %20936 12 %13157 13 %13773
%13773 = OpLabel
%19312 = OpFMul %float %12281 %float_4574_43115
%11539 = OpExtInst %float %1 Fract %19312
%20509 = OpFSub %float %11539 %float_0_5
%10174 = OpFMul %float %20509 %float_2
%24990 = OpCompositeExtract %float %19276 2
%15735 = OpExtInst %float %1 FSign %10174
%18641 = OpConvertFToS %int %15735
%25205 = OpConvertSToF %float %18641
%13281 = OpExtInst %float %1 Fma %10174 %24990 %25205
OpBranch %7164
%13157 = OpLabel
%24180 = OpCompositeExtract %float %19276 2
%24951 = OpFMul %float %12281 %float_4574_43115
%23801 = OpExtInst %float %1 Fract %24951
%24368 = OpExtInst %float %1 Fma %24180 %23801 %float_1
OpBranch %7164
%20936 = OpLabel
%17699 = OpFMul %float %12281 %float_4574_43115
%11540 = OpExtInst %float %1 Fract %17699
%23701 = OpFSub %float %11540 %float_0_5
%23730 = OpFMul %float %23701 %float_2
OpBranch %7164
%6271 = OpLabel
%19941 = OpFMul %float %12281 %float_4574_43115
%12379 = OpExtInst %float %1 Fract %19941
OpBranch %7164
%13015 = OpLabel
%8639 = OpCompositeExtract %float %13449 3
OpBranch %7164
%10175 = OpLabel
%8640 = OpCompositeExtract %float %13449 2
OpBranch %7164
%10176 = OpLabel
%8641 = OpCompositeExtract %float %13449 1
OpBranch %7164
%10177 = OpLabel
%8642 = OpCompositeExtract %float %13446 3
OpBranch %7164
%10178 = OpLabel
%8643 = OpCompositeExtract %float %13444 3
OpBranch %7164
%10179 = OpLabel
%8644 = OpCompositeExtract %float %19276 3
OpBranch %7164
%10180 = OpLabel
%8645 = OpCompositeExtract %float %19276 1
OpBranch %7164
%6618 = OpLabel
%17787 = OpCompositeExtract %float %19276 2
%12038 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19806 = OpLoad %float %12038
%13238 = OpExtInst %float %1 Fma %17787 %19806 %float_1
OpBranch %7164
%13082 = OpLabel
%10287 = OpCompositeExtract %float %19276 2
%25269 = OpFAdd %float %10287 %float_1
OpBranch %7164
%7164 = OpLabel
%13184 = OpPhi %float %2 %17660 %13281 %13773 %24368 %13157 %23730 %20936 %12379 %6271 %8639 %13015 %8640 %10175 %8641 %10176 %8642 %10177 %8643 %10178 %8644 %10179 %8645 %10180 %13238 %6618 %25269 %13082
%16064 = OpPhi %bool %false %17660 %true %13773 %true %13157 %true %20936 %true %6271 %true %13015 %true %10175 %true %10176 %true %10177 %true %10178 %true %10179 %true %10180 %true %6618 %true %13082
OpSelectionMerge %23963 None
OpBranchConditional %16064 %19320 %23963
%23963 = OpLabel
OpBranch %19320
%19320 = OpLabel
%22341 = OpPhi %float %13184 %7164 %float_1 %23963
%10003 = OpCompositeInsert %v4float %22341 %12469 3
OpBranch %11044
%15703 = OpLabel
OpUnreachable
%11044 = OpLabel
%20091 = OpPhi %v4float %18602 %19299 %10003 %19320
%7546 = OpAccessChain %_ptr_Uniform_float %3805 %uint_2 %int_2
%6701 = OpLoad %float %7546
%7758 = OpConvertFToS %int %6701
%24637 = OpShiftRightArithmetic %int %7758 %int_7
%17527 = OpBitwiseAnd %int %24637 %int_1
%19223 = OpIEqual %bool %17527 %int_1
OpSelectionMerge %15031 None
OpBranchConditional %19223 %15240 %15031
%15240 = OpLabel
%9919 = OpVectorShuffle %v4float %600 %16427 0 1 4 5
OpBranch %15031
%15031 = OpLabel
%20655 = OpPhi %v4float %600 %11044 %9919 %15240
%12073 = OpVectorShuffle %v3float %13444 %13444 0 1 2
%16264 = OpFMul %v3float %12073 %418
%11701 = OpExtInst %v3float %1 Fma %12073 %1533 %799
%24533 = OpExtInst %v3float %1 Pow %11701 %2947
%22261 = OpCompositeExtract %float %13444 0
%19001 = OpFOrdLessThanEqual %bool %22261 %float_0_0404499993
%20392 = OpCompositeExtract %float %16264 0
%6437 = OpCompositeExtract %float %24533 0
%8911 = OpSelect %float %19001 %20392 %6437
%11697 = OpCompositeExtract %float %13444 1
%10181 = OpFOrdLessThanEqual %bool %11697 %float_0_0404499993
%20393 = OpCompositeExtract %float %16264 1
%6438 = OpCompositeExtract %float %24533 1
%8912 = OpSelect %float %10181 %20393 %6438
%11698 = OpCompositeExtract %float %13444 2
%10182 = OpFOrdLessThanEqual %bool %11698 %float_0_0404499993
%20394 = OpCompositeExtract %float %16264 2
%6439 = OpCompositeExtract %float %24533 2
%6859 = OpSelect %float %10182 %20394 %6439
%13476 = OpCompositeExtract %float %13444 3
%6736 = OpCompositeConstruct %v4float %8911 %8912 %6859 %13476
%22188 = OpShiftRightArithmetic %int %7757 %int_8
%19737 = OpBitwiseAnd %int %22188 %int_15
%12108 = OpConvertSToF %float %19737
%6353 = OpCompositeConstruct %v4float %12108 %12108 %12108 %12108
%7415 = OpExtInst %v4float %1 FMix %13444 %6736 %6353
%7632 = OpVectorShuffle %v3float %13451 %13451 0 1 2
%10183 = OpFMul %v3float %7632 %418
%11702 = OpExtInst %v3float %1 Fma %7632 %1533 %799
%24534 = OpExtInst %v3float %1 Pow %11702 %2947
%22262 = OpCompositeExtract %float %13451 0
%19002 = OpFOrdLessThanEqual %bool %22262 %float_0_0404499993
%20395 = OpCompositeExtract %float %10183 0
%6440 = OpCompositeExtract %float %24534 0
%8913 = OpSelect %float %19002 %20395 %6440
%11699 = OpCompositeExtract %float %13451 1
%10184 = OpFOrdLessThanEqual %bool %11699 %float_0_0404499993
%20396 = OpCompositeExtract %float %10183 1
%6441 = OpCompositeExtract %float %24534 1
%8914 = OpSelect %float %10184 %20396 %6441
%11700 = OpCompositeExtract %float %13451 2
%10185 = OpFOrdLessThanEqual %bool %11700 %float_0_0404499993
%20397 = OpCompositeExtract %float %10183 2
%6442 = OpCompositeExtract %float %24534 2
%6860 = OpSelect %float %10185 %20397 %6442
%10045 = OpCompositeExtract %float %13451 3
%9618 = OpCompositeConstruct %v4float %8913 %8914 %6860 %10045
%6622 = OpExtInst %v4float %1 FMix %13451 %9618 %6353
%14150 = OpCompositeConstruct %v4float %12755 %12755 %12755 %12755
%24417 = OpExtInst %v4float %1 FMix %7415 %6622 %14150
%19224 = OpVectorShuffle %v3float %13445 %13445 0 1 2
%15821 = OpVectorShuffle %v3float %13446 %13446 0 1 2
%13095 = OpFMul %float %12755 %12755
%17520 = OpFMul %float %13095 %float_0_5
%22215 = OpFMul %float %12755 %17520
%22379 = OpFMul %float %12755 %float_0_5
%23270 = OpFNegate %float %22215
%23152 = OpCompositeExtract %float %13445 0
%21563 = OpFMul %float %23152 %23270
%24619 = OpCompositeExtract %float %13445 1
%7331 = OpFMul %float %24619 %23270
%6536 = OpCompositeExtract %float %13445 2
%16418 = OpFMul %float %6536 %23270
%8070 = OpFMul %float %22215 %float_3
%24197 = OpCompositeExtract %float %13446 0
%13805 = OpCompositeExtract %float %13446 1
%13239 = OpCompositeExtract %float %13446 2
%15761 = OpFMul %float %22215 %float_n3
%14544 = OpCompositeExtract %float %13447 0
%12817 = OpCompositeExtract %float %13447 1
%22235 = OpCompositeExtract %float %13447 2
%22236 = OpCompositeExtract %float %13448 0
%20974 = OpCompositeExtract %float %13448 1
%12381 = OpCompositeExtract %float %13448 2
%17518 = OpExtInst %float %1 Fma %24197 %8070 %21563
%21523 = OpExtInst %float %1 Fma %13805 %8070 %7331
%20313 = OpExtInst %float %1 Fma %13239 %8070 %16418
%20314 = OpExtInst %float %1 Fma %14544 %15761 %17518
%20315 = OpExtInst %float %1 Fma %12817 %15761 %21523
%20316 = OpExtInst %float %1 Fma %22235 %15761 %20313
%20317 = OpExtInst %float %1 Fma %22236 %22215 %20314
%22562 = OpExtInst %float %1 Fma %20974 %22215 %20315
%22159 = OpExtInst %float %1 Fma %12381 %22215 %20316
%10548 = OpFMul %float %13095 %float_n2_5
%15674 = OpFMul %float %13095 %float_2
%20483 = OpFMul %float %13095 %float_n0_5
%19480 = OpExtInst %float %1 Fma %23152 %13095 %20317
%24719 = OpExtInst %float %1 Fma %24619 %13095 %22562
%20318 = OpExtInst %float %1 Fma %6536 %13095 %22159
%20319 = OpExtInst %float %1 Fma %24197 %10548 %19480
%20320 = OpExtInst %float %1 Fma %13805 %10548 %24719
%20321 = OpExtInst %float %1 Fma %13239 %10548 %20318
%20322 = OpExtInst %float %1 Fma %14544 %15674 %20319
%20323 = OpExtInst %float %1 Fma %12817 %15674 %20320
%20324 = OpExtInst %float %1 Fma %22235 %15674 %20321
%20325 = OpExtInst %float %1 Fma %22236 %20483 %20322
%22563 = OpExtInst %float %1 Fma %20974 %20483 %20323
%19910 = OpExtInst %float %1 Fma %12381 %20483 %20324
%8702 = OpFMul %float %12755 %float_n0_5
%15074 = OpExtInst %float %1 Fma %23152 %8702 %20325
%24720 = OpExtInst %float %1 Fma %24619 %8702 %22563
%20326 = OpExtInst %float %1 Fma %6536 %8702 %19910
%20327 = OpExtInst %float %1 Fma %14544 %22379 %15074
%22486 = OpExtInst %float %1 Fma %12817 %22379 %24720
%22775 = OpExtInst %float %1 Fma %22235 %22379 %20326
%24865 = OpFAdd %float %24197 %20327
%19691 = OpFAdd %float %13805 %22486
%17741 = OpFAdd %float %13239 %22775
%9538 = OpCompositeConstruct %v3float %24865 %19691 %17741
%14429 = OpFMul %float %float_3 %12755
%15098 = OpFMul %float %14429 %12755
%22329 = OpFMul %float %15098 %float_0_5
%22330 = OpFMul %float %15098 %float_n0_5
%22338 = OpFMul %float %23152 %22330
%22339 = OpFMul %float %24619 %22330
%22342 = OpFMul %float %6536 %22330
%20080 = OpFMul %float %15098 %float_1_5
%20484 = OpFMul %float %15098 %float_n1_5
%19481 = OpExtInst %float %1 Fma %24197 %20080 %22338
%24721 = OpExtInst %float %1 Fma %13805 %20080 %22339
%20328 = OpExtInst %float %1 Fma %13239 %20080 %22342
%20329 = OpExtInst %float %1 Fma %14544 %20484 %19481
%20330 = OpExtInst %float %1 Fma %12817 %20484 %24721
%20331 = OpExtInst %float %1 Fma %22235 %20484 %20328
%20332 = OpExtInst %float %1 Fma %22236 %22329 %20329
%22564 = OpExtInst %float %1 Fma %20974 %22329 %20330
%22160 = OpExtInst %float %1 Fma %12381 %22329 %20331
%10549 = OpFMul %float %12755 %float_2
%17809 = OpFMul %float %12755 %float_n5
%21118 = OpFMul %float %12755 %float_4
%12428 = OpFNegate %float %12755
%8360 = OpExtInst %float %1 Fma %23152 %10549 %20332
%7266 = OpExtInst %float %1 Fma %24619 %10549 %22564
%20333 = OpExtInst %float %1 Fma %6536 %10549 %22160
%20334 = OpExtInst %float %1 Fma %24197 %17809 %8360
%20335 = OpExtInst %float %1 Fma %13805 %17809 %7266
%20336 = OpExtInst %float %1 Fma %13239 %17809 %20333
%20337 = OpExtInst %float %1 Fma %14544 %21118 %20334
%20338 = OpExtInst %float %1 Fma %12817 %21118 %20335
%20339 = OpExtInst %float %1 Fma %22235 %21118 %20336
%20340 = OpExtInst %float %1 Fma %22236 %12428 %20337
%20341 = OpExtInst %float %1 Fma %20974 %12428 %20338
%20342 = OpExtInst %float %1 Fma %12381 %12428 %20339
%20343 = OpExtInst %float %1 Fma %23152 %float_n0_5 %20340
%20345 = OpExtInst %float %1 Fma %24619 %float_n0_5 %20341
%20346 = OpExtInst %float %1 Fma %6536 %float_n0_5 %20342
%20347 = OpExtInst %float %1 Fma %14544 %float_0_5 %20343
%21555 = OpExtInst %float %1 Fma %12817 %float_0_5 %20345
%11250 = OpExtInst %float %1 Fma %22235 %float_0_5 %20346
%13060 = OpCompositeConstruct %v3float %20347 %21555 %11250
%20103 = OpCompositeConstruct %v3float %12755 %12755 %12755
%12742 = OpExtInst %v3float %1 FMix %13519 %12751 %20103
%16159 = OpExtInst %v3float %1 Normalize %12742
%10367 = OpExtInst %v3float %1 Normalize %13519
%24778 = OpFSub %v3float %23486 %19224
%20984 = OpExtInst %v3float %1 Normalize %24778
%11179 = OpDot %float %10367 %20984
%16202 = OpExtInst %float %1 FAbs %11179
%17217 = OpAccessChain %_ptr_Uniform_float %3553 %int_2 %int_0
%21830 = OpLoad %float %17217
%21157 = OpAccessChain %_ptr_Uniform_float %3553 %int_2 %int_1
%22380 = OpLoad %float %21157
%22711 = OpExtInst %float %1 SmoothStep %21830 %22380 %16202
%16716 = OpFSub %float %float_1 %22711
%22817 = OpExtInst %v3float %1 Normalize %12751
%10922 = OpFSub %v3float %23486 %15821
%20985 = OpExtInst %v3float %1 Normalize %10922
%10221 = OpDot %float %22817 %20985
%14210 = OpExtInst %float %1 FAbs %10221
%17790 = OpExtInst %float %1 SmoothStep %21830 %22380 %14210
%7414 = OpFSub %float %float_1 %17790
%15526 = OpExtInst %float %1 FMix %16716 %7414 %12755
%23672 = OpCompositeExtract %float %24417 3
%11669 = OpFMul %float %23672 %15526
%23453 = OpCompositeInsert %v4float %11669 %24417 3
%6844 = OpExtInst %v3float %1 Cross %13060 %16159
%12109 = OpExtInst %v3float %1 Normalize %6844
%25221 = OpCompositeExtract %float %13445 3
%25062 = OpCompositeExtract %float %13446 3
%23223 = OpCompositeExtract %float %13447 3
%14113 = OpCompositeExtract %float %13448 3
%8715 = OpFMul %float %13095 %12755
%11770 = OpFNegate %float %25221
%10047 = OpFAdd %float %11770 %23223
%12732 = OpFMul %float %10047 %12755
%15851 = OpExtInst %float %1 Fma %float_2 %25062 %12732
%13108 = OpFMul %float %25062 %float_n5
%17285 = OpExtInst %float %1 Fma %float_2 %25221 %13108
%24662 = OpExtInst %float %1 Fma %float_4 %23223 %17285
%25088 = OpFSub %float %24662 %14113
%15572 = OpExtInst %float %1 Fma %25088 %13095 %15851
%12665 = OpExtInst %float %1 Fma %float_3 %25062 %11770
%20602 = OpExtInst %float %1 Fma %float_n3 %23223 %12665
%24652 = OpFAdd %float %20602 %14113
%20082 = OpExtInst %float %1 Fma %24652 %8715 %15572
%13300 = OpFMul %float %float_0_5 %20082
OpSelectionMerge %18185 None
OpBranchConditional %19223 %23844 %13207
%13207 = OpLabel
%25219 = OpVectorShuffle %v2float %20091 %20091 0 2
%9136 = OpVectorTimesScalar %v2float %25219 %13300
%14297 = OpVectorShuffle %v4float %20091 %9136 4 1 5 3
%10005 = OpCompositeConstruct %v2float %float_1 %13300
%17823 = OpVectorShuffle %v4float %20655 %10005 0 1 4 5
OpBranch %18185
%23844 = OpLabel
%10494 = OpBitwiseAnd %int %7757 %int_15
%21931 = OpBitcast %uint %10494
OpSelectionMerge %20249 None
OpSwitch %uint_0 %17661
%17661 = OpLabel
OpSelectionMerge %7165 None
OpSwitch %21931 %7165 1 %13083 2 %6619 3 %10189 4 %12721 5 %21780 6 %19163 7 %10188 8 %10187 9 %13016 10 %17560 11 %13158 12 %12166 13 %25063
%25063 = OpLabel
%6712 = OpCompositeExtract %float %19276 0
%8102 = OpFMul %float %6712 %float_238_839996
%21544 = OpExtInst %float %1 Fract %8102
%20510 = OpFSub %float %21544 %float_0_5
%10186 = OpFMul %float %20510 %float_2
%24991 = OpCompositeExtract %float %19276 2
%15736 = OpExtInst %float %1 FSign %10186
%18642 = OpConvertFToS %int %15736
%25206 = OpConvertSToF %float %18642
%13282 = OpExtInst %float %1 Fma %10186 %24991 %25206
OpBranch %7165
%12166 = OpLabel
%16384 = OpCompositeExtract %float %19276 2
%21019 = OpCompositeExtract %float %19276 0
%22343 = OpFMul %float %21019 %float_238_839996
%23802 = OpExtInst %float %1 Fract %22343
%24369 = OpExtInst %float %1 Fma %16384 %23802 %float_1
OpBranch %7165
%13158 = OpLabel
%24181 = OpCompositeExtract %float %19276 0
%8103 = OpFMul %float %24181 %float_238_839996
%21545 = OpExtInst %float %1 Fract %8103
%23702 = OpFSub %float %21545 %float_0_5
%23731 = OpFMul %float %23702 %float_2
OpBranch %7165
%17560 = OpLabel
%24182 = OpCompositeExtract %float %19276 0
%10337 = OpFMul %float %24182 %float_238_839996
%22381 = OpExtInst %float %1 Fract %10337
OpBranch %7165
%13016 = OpLabel
%8646 = OpCompositeExtract %float %13449 3
OpBranch %7165
%10187 = OpLabel
%8647 = OpCompositeExtract %float %13449 2
OpBranch %7165
%10188 = OpLabel
%8648 = OpCompositeExtract %float %13449 1
OpBranch %7165
%19163 = OpLabel
OpBranch %7165
%21780 = OpLabel
OpBranch %7165
%12721 = OpLabel
%8649 = OpCompositeExtract %float %19276 3
OpBranch %7165
%10189 = OpLabel
%8650 = OpCompositeExtract %float %19276 1
OpBranch %7165
%6619 = OpLabel
%17788 = OpCompositeExtract %float %19276 2
%12039 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19807 = OpLoad %float %12039
%13240 = OpExtInst %float %1 Fma %17788 %19807 %float_1
OpBranch %7165
%13083 = OpLabel
%10288 = OpCompositeExtract %float %19276 2
%25270 = OpFAdd %float %10288 %float_1
OpBranch %7165
%7165 = OpLabel
%13185 = OpPhi %float %2 %17661 %13282 %25063 %24369 %12166 %23731 %13158 %22381 %17560 %8646 %13016 %8647 %10187 %8648 %10188 %25062 %19163 %13476 %21780 %8649 %12721 %8650 %10189 %13240 %6619 %25270 %13083
%16065 = OpPhi %bool %false %17661 %true %25063 %true %12166 %true %13158 %true %17560 %true %13016 %true %10187 %true %10188 %true %19163 %true %21780 %true %12721 %true %10189 %true %6619 %true %13083
OpSelectionMerge %23964 None
OpBranchConditional %16065 %20249 %23964
%23964 = OpLabel
OpBranch %20249
%20249 = OpLabel
%12837 = OpPhi %float %13185 %7165 %float_1 %23964
%10990 = OpFSub %float %float_1 %12837
%18419 = OpShiftRightArithmetic %int %7757 %int_4
%7208 = OpBitwiseAnd %int %18419 %int_15
%14701 = OpBitcast %uint %7208
OpSelectionMerge %19283 None
OpSwitch %uint_0 %17662
%17662 = OpLabel
OpSelectionMerge %7166 None
OpSwitch %14701 %7166 1 %13084 2 %6620 3 %10193 4 %12722 5 %21781 6 %19164 7 %10192 8 %10191 9 %13017 10 %17561 11 %13159 12 %12167 13 %25064
%25064 = OpLabel
%6714 = OpCompositeExtract %float %19276 0
%8104 = OpFMul %float %6714 %float_84_7455978
%21546 = OpExtInst %float %1 Fract %8104
%20511 = OpFSub %float %21546 %float_0_5
%10190 = OpFMul %float %20511 %float_2
%24992 = OpCompositeExtract %float %19276 2
%15737 = OpExtInst %float %1 FSign %10190
%18643 = OpConvertFToS %int %15737
%25207 = OpConvertSToF %float %18643
%13283 = OpExtInst %float %1 Fma %10190 %24992 %25207
OpBranch %7166
%12167 = OpLabel
%16385 = OpCompositeExtract %float %19276 2
%21020 = OpCompositeExtract %float %19276 0
%22344 = OpFMul %float %21020 %float_84_7455978
%23803 = OpExtInst %float %1 Fract %22344
%24370 = OpExtInst %float %1 Fma %16385 %23803 %float_1
OpBranch %7166
%13159 = OpLabel
%24183 = OpCompositeExtract %float %19276 0
%8105 = OpFMul %float %24183 %float_84_7455978
%21547 = OpExtInst %float %1 Fract %8105
%23703 = OpFSub %float %21547 %float_0_5
%23732 = OpFMul %float %23703 %float_2
OpBranch %7166
%17561 = OpLabel
%24184 = OpCompositeExtract %float %19276 0
%10338 = OpFMul %float %24184 %float_84_7455978
%22382 = OpExtInst %float %1 Fract %10338
OpBranch %7166
%13017 = OpLabel
%8651 = OpCompositeExtract %float %13449 3
OpBranch %7166
%10191 = OpLabel
%8652 = OpCompositeExtract %float %13449 2
OpBranch %7166
%10192 = OpLabel
%8653 = OpCompositeExtract %float %13449 1
OpBranch %7166
%19164 = OpLabel
OpBranch %7166
%21781 = OpLabel
OpBranch %7166
%12722 = OpLabel
%8654 = OpCompositeExtract %float %19276 3
OpBranch %7166
%10193 = OpLabel
%8655 = OpCompositeExtract %float %19276 1
OpBranch %7166
%6620 = OpLabel
%17789 = OpCompositeExtract %float %19276 2
%12040 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_3
%19808 = OpLoad %float %12040
%13241 = OpExtInst %float %1 Fma %17789 %19808 %float_1
OpBranch %7166
%13084 = OpLabel
%10289 = OpCompositeExtract %float %19276 2
%25271 = OpFAdd %float %10289 %float_1
OpBranch %7166
%7166 = OpLabel
%13186 = OpPhi %float %2 %17662 %13283 %25064 %24370 %12167 %23732 %13159 %22382 %17561 %8651 %13017 %8652 %10191 %8653 %10192 %25062 %19164 %13476 %21781 %8654 %12722 %8655 %10193 %13241 %6620 %25271 %13084
%16066 = OpPhi %bool %false %17662 %true %25064 %true %12167 %true %13159 %true %17561 %true %13017 %true %10191 %true %10192 %true %19164 %true %21781 %true %12722 %true %10193 %true %6620 %true %13084
OpSelectionMerge %23965 None
OpBranchConditional %16066 %19283 %23965
%23965 = OpLabel
OpBranch %19283
%19283 = OpLabel
%19456 = OpPhi %float %13186 %7166 %float_1 %23965
%17675 = OpCompositeConstruct %v2float %10990 %19456
%22862 = OpVectorShuffle %v4float %20655 %17675 0 1 4 5
OpBranch %18185
%18185 = OpLabel
%10970 = OpPhi %v4float %14297 %13207 %20091 %19283
%12595 = OpPhi %v4float %17823 %13207 %22862 %19283
%25255 = OpFSub %v3float %23486 %9538
%23480 = OpExtInst %float %1 Length %25255
%18322 = OpAccessChain %_ptr_Uniform_float %3553 %int_2 %int_2
%8683 = OpLoad %float %18322
%6405 = OpFOrdGreaterThan %bool %8683 %float_0
OpSelectionMerge %10595 None
OpBranchConditional %6405 %14090 %10595
%14090 = OpLabel
%21463 = OpAccessChain %_ptr_Uniform_float %3553 %int_0 %int_1
%20813 = OpLoad %float %21463
%12497 = OpFMul %float %20813 %23480
%15436 = OpFOrdGreaterThan %bool %13300 %12497
OpSelectionMerge %19809 None
OpBranchConditional %15436 %14091 %19809
%14091 = OpLabel
%21464 = OpAccessChain %_ptr_Uniform_float %3553 %int_0 %int_2
%20814 = OpLoad %float %21464
%12498 = OpFMul %float %20814 %23480
%15437 = OpFOrdGreaterThan %bool %13300 %12498
OpSelectionMerge %22491 None
OpBranchConditional %15437 %7318 %21554
%21554 = OpLabel
%9866 = OpFNegate %float %12497
%9973 = OpExtInst %float %1 Fma %float_0_5 %20082 %9866
%13944 = OpFSub %float %20814 %20813
%24321 = OpFMul %float %23480 %13944
%15738 = OpFDiv %float %9973 %24321
%19544 = OpFSub %float %float_1 %15738
%13139 = OpVectorTimesScalar %v4float %23453 %19544
OpBranch %22491
%7318 = OpLabel
OpBranch %22491
%22491 = OpLabel
%12501 = OpPhi %v4float %13139 %21554 %2938 %7318
OpBranch %19809
%19809 = OpLabel
%21709 = OpPhi %v4float %23453 %14090 %12501 %22491
%7983 = OpAccessChain %_ptr_Uniform_float %3553 %int_0 %int_0
%18356 = OpLoad %float %7983
%22717 = OpFMul %float %18356 %23480
%16463 = OpExtInst %float %1 FMax %13300 %22717
%24319 = OpAccessChain %_ptr_Uniform_float %3553 %int_0 %int_3
%10052 = OpLoad %float %24319
%12188 = OpFMul %float %10052 %23480
%12935 = OpExtInst %float %1 FMin %16463 %12188
OpBranch %10595
%10595 = OpLabel
%11180 = OpPhi %float %13300 %18185 %12935 %19809
%11937 = OpPhi %v4float %23453 %18185 %21709 %19809
%16599 = OpVectorTimesScalar %v3float %12109 %11180
%12245 = OpCompositeExtract %float %22883 0
%17230 = OpVectorTimesScalar %v3float %16599 %12245
%6621 = OpFAdd %v3float %9538 %17230
%10194 = OpCompositeExtract %float %6621 0
%9572 = OpCompositeExtract %float %6621 1
%22104 = OpCompositeExtract %float %6621 2
%13745 = OpCompositeConstruct %v4float %10194 %9572 %22104 %float_1
%12080 = OpAccessChain %_ptr_Uniform_v4float %4459 %uint_4 %uint_0
%17978 = OpLoad %v4float %12080
%21447 = OpVectorTimesScalar %v4float %17978 %float_1
%24859 = OpFAdd %v4float %13745 %21447
%12400 = OpAccessChain %_ptr_Uniform_mat4v4float %4459 %uint_3 %uint_0
%20421 = OpLoad %mat4v4float %12400
%16067 = OpVectorTimesMatrix %v4float %24859 %20421
%15493 = OpAccessChain %_ptr_Uniform_v3float %4459 %uint_2
%17353 = OpLoad %v3float %15493
%23578 = OpAccessChain %_ptr_Uniform_float %3553 %int_1 %int_0
%12340 = OpLoad %float %23578
%21751 = OpVectorTimesScalar %v3float %17353 %12340
%22471 = OpFAdd %v3float %6621 %21751
%10195 = OpCompositeExtract %float %22471 0
%9573 = OpCompositeExtract %float %22471 1
%23320 = OpCompositeExtract %float %22471 2
%23321 = OpCompositeConstruct %v4float %10195 %9573 %23320 %float_1
%18812 = OpFAdd %v4float %23321 %21447
%16720 = OpVectorTimesMatrix %v4float %18812 %20421
%24389 = OpCompositeExtract %float %16067 3
%13048 = OpCompositeExtract %float %16720 2
%13708 = OpCompositeExtract %float %16720 3
%20348 = OpFDiv %float %13048 %13708
%18663 = OpFMul %float %24389 %20348
%22676 = OpCompositeExtract %float %16067 2
%16776 = OpExtInst %float %1 FMin %float_0_00100000005 %22676
%21936 = OpExtInst %float %1 FMax %18663 %16776
%15598 = OpCompositeInsert %v4float %21936 %16067 2
%7049 = OpVectorShuffle %v4float %13357 %6621 4 5 6 3
%25108 = OpFSub %v3float %6621 %23486
%16143 = OpDot %float %17353 %25108
%12318 = OpCompositeInsert %v4float %16143 %7049 3
%8214 = OpExtInst %v3float %1 FaceForward %626 %25108 %626
%8769 = OpAccessChain %_ptr_Uniform_v4float %4459 %uint_0
%19722 = OpLoad %v4float %8769
%21427 = OpDot %float %13745 %19722
%8307 = OpAccessChain %_ptr_Uniform_float %3805 %uint_1 %int_2
%19696 = OpLoad %float %8307
%20440 = OpVectorShuffle %v3float %11937 %11937 0 1 2
%17979 = OpVectorTimesScalar %v3float %20440 %19696
%17860 = OpAccessChain %_ptr_Uniform_float %3805 %uint_0 %int_2
%14857 = OpLoad %float %17860
%20706 = OpVectorShuffle %v3float %17979 %212 0 1 2
%15575 = OpVectorTimesScalar %v3float %20706 %14857
%17088 = OpVectorShuffle %v4float %11937 %15575 4 5 6 3
OpStore %3486 %10970
OpStore %5188 %17088
OpStore %3488 %12318
OpStore %3489 %8214
OpStore %3490 %12595
%7532 = OpCompositeExtract %float %16067 1
%20605 = OpFNegate %float %7532
%17286 = OpCompositeInsert %v4float %20605 %15598 1
OpStore %gl_Position %17286
%22833 = OpAccessChain %_ptr_Output_float %gl_ClipDistance %uint_0
OpStore %22833 %21427
OpReturn
OpFunctionEnd
