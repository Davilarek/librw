#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//   fxc /T vs_2_0 /DDIRECTIONALS /Fh default_amb_dir_VS.h default_VS.hlsl
//
//
// Parameters:
//
//   float4 ambientLight;
//   float4x4 combinedMat;
//   int4 firstLight;
//   float4 fogData;
//   
//   struct
//   {
//       float4 color;
//       float4 position;
//       float4 direction;
//
//   } lights[8];
//   
//   float4 matCol;
//   float3x3 normalMat;
//   int numDirLights;
//   float4 surfProps;
//
//
// Registers:
//
//   Name         Reg   Size
//   ------------ ----- ----
//   numDirLights i0       1
//   combinedMat  c0       4
//   normalMat    c8       3
//   matCol       c12      1
//   surfProps    c13      1
//   fogData      c14      1
//   ambientLight c15      1
//   firstLight   c16      1
//   lights       c17     24
//

    vs_2_0
    def c4, 3, 0, 0, 0
    def c5, 0, -0, -1, 1
    dcl_position v0
    dcl_normal v1
    dcl_texcoord v2
    dcl_color v3
    mul r0, v0.y, c1
    mad r0, c0, v0.x, r0
    mad r0, c2, v0.z, r0
    mad r0, c3, v0.w, r0
    mov oPos, r0
    mul r0.xyz, v1.y, c9
    mad r0.xyz, c8, v1.x, r0
    mad r0.xyz, c10, v1.z, r0
    mov r1.x, c5.x
    slt r1.x, r1.x, c13.w
    add r2, v3, c5.yyyz
    mad r1, r1.x, r2, c5.xxxw
    mov r2.x, c13.x
    mad r2.xyz, c15, r2.x, r1
    mov r1.xyz, r2
    mov r2.w, c5.x
    rep i0
      add r3.x, r2.w, c16.x
      mul r3.x, r3.x, c4.x
      mova a0.x, r3.x
      dp3 r3.x, r0, -c19[a0.x]
      max r3.x, r3.x, c5.x
      mul r3.xyz, r3.x, c17[a0.x]
      mad r1.xyz, r3, c13.z, r1
      add r2.w, r2.w, c5.w
    endrep
    max r1, r1, c5.x
    min r1, r1, c5.w
    mul oD0, r1, c12
    add r0.x, r0.w, -c14.y
    mul r0.x, r0.x, c14.z
    max r0.x, r0.x, c14.w
    min oT0.z, r0.x, c5.w
    mov oT0.xy, v2

// approximately 37 instruction slots used
#endif

const BYTE g_vs20_main[] =
{
      0,   2, 254, 255, 254, 255, 
    134,   0,  67,  84,  65,  66, 
     28,   0,   0,   0, 225,   1, 
      0,   0,   0,   2, 254, 255, 
      9,   0,   0,   0,  28,   0, 
      0,   0,   0,   1,   0,   0, 
    218,   1,   0,   0, 208,   0, 
      0,   0,   2,   0,  15,   0, 
      1,   0,  62,   0, 224,   0, 
      0,   0,   0,   0,   0,   0, 
    240,   0,   0,   0,   2,   0, 
      0,   0,   4,   0,   2,   0, 
    252,   0,   0,   0,   0,   0, 
      0,   0,  12,   1,   0,   0, 
      2,   0,  16,   0,   1,   0, 
     66,   0,  24,   1,   0,   0, 
      0,   0,   0,   0,  40,   1, 
      0,   0,   2,   0,  14,   0, 
      1,   0,  58,   0, 224,   0, 
      0,   0,   0,   0,   0,   0, 
     48,   1,   0,   0,   2,   0, 
     17,   0,  24,   0,  70,   0, 
    124,   1,   0,   0,   0,   0, 
      0,   0, 140,   1,   0,   0, 
      2,   0,  12,   0,   1,   0, 
     50,   0, 224,   0,   0,   0, 
      0,   0,   0,   0, 147,   1, 
      0,   0,   2,   0,   8,   0, 
      3,   0,  34,   0, 160,   1, 
      0,   0,   0,   0,   0,   0, 
    176,   1,   0,   0,   1,   0, 
      0,   0,   1,   0,   2,   0, 
    192,   1,   0,   0,   0,   0, 
      0,   0, 208,   1,   0,   0, 
      2,   0,  13,   0,   1,   0, 
     54,   0, 224,   0,   0,   0, 
      0,   0,   0,   0,  97, 109, 
     98, 105, 101, 110, 116,  76, 
    105, 103, 104, 116,   0, 171, 
    171, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     99, 111, 109,  98, 105, 110, 
    101, 100,  77,  97, 116,   0, 
      3,   0,   3,   0,   4,   0, 
      4,   0,   1,   0,   0,   0, 
      0,   0,   0,   0, 102, 105, 
    114, 115, 116,  76, 105, 103, 
    104, 116,   0, 171,   1,   0, 
      2,   0,   1,   0,   4,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0, 102, 111, 103,  68, 
     97, 116,  97,   0, 108, 105, 
    103, 104, 116, 115,   0,  99, 
    111, 108, 111, 114,   0, 171, 
    171, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
    112, 111, 115, 105, 116, 105, 
    111, 110,   0, 100, 105, 114, 
    101,  99, 116, 105, 111, 110, 
      0, 171,  55,   1,   0,   0, 
     64,   1,   0,   0,  80,   1, 
      0,   0,  64,   1,   0,   0, 
     89,   1,   0,   0,  64,   1, 
      0,   0,   5,   0,   0,   0, 
      1,   0,  12,   0,   8,   0, 
      3,   0, 100,   1,   0,   0, 
    109,  97, 116,  67, 111, 108, 
      0, 110, 111, 114, 109,  97, 
    108,  77,  97, 116,   0, 171, 
    171, 171,   3,   0,   3,   0, 
      3,   0,   3,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
    110, 117, 109,  68, 105, 114, 
     76, 105, 103, 104, 116, 115, 
      0, 171, 171, 171,   0,   0, 
      2,   0,   1,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0, 115, 117, 114, 102, 
     80, 114, 111, 112, 115,   0, 
    118, 115,  95,  50,  95,  48, 
      0,  77, 105,  99, 114, 111, 
    115, 111, 102, 116,  32,  40, 
     82,  41,  32,  72,  76,  83, 
     76,  32,  83, 104,  97, 100, 
    101, 114,  32,  67, 111, 109, 
    112, 105, 108, 101, 114,  32, 
     57,  46,  50,  57,  46,  57, 
     53,  50,  46,  51,  49,  49, 
     49,   0, 171, 171,  81,   0, 
      0,   5,   4,   0,  15, 160, 
      0,   0,  64,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  81,   0, 
      0,   5,   5,   0,  15, 160, 
      0,   0,   0,   0,   0,   0, 
      0, 128,   0,   0, 128, 191, 
      0,   0, 128,  63,  31,   0, 
      0,   2,   0,   0,   0, 128, 
      0,   0,  15, 144,  31,   0, 
      0,   2,   3,   0,   0, 128, 
      1,   0,  15, 144,  31,   0, 
      0,   2,   5,   0,   0, 128, 
      2,   0,  15, 144,  31,   0, 
      0,   2,  10,   0,   0, 128, 
      3,   0,  15, 144,   5,   0, 
      0,   3,   0,   0,  15, 128, 
      0,   0,  85, 144,   1,   0, 
    228, 160,   4,   0,   0,   4, 
      0,   0,  15, 128,   0,   0, 
    228, 160,   0,   0,   0, 144, 
      0,   0, 228, 128,   4,   0, 
      0,   4,   0,   0,  15, 128, 
      2,   0, 228, 160,   0,   0, 
    170, 144,   0,   0, 228, 128, 
      4,   0,   0,   4,   0,   0, 
     15, 128,   3,   0, 228, 160, 
      0,   0, 255, 144,   0,   0, 
    228, 128,   1,   0,   0,   2, 
      0,   0,  15, 192,   0,   0, 
    228, 128,   5,   0,   0,   3, 
      0,   0,   7, 128,   1,   0, 
     85, 144,   9,   0, 228, 160, 
      4,   0,   0,   4,   0,   0, 
      7, 128,   8,   0, 228, 160, 
      1,   0,   0, 144,   0,   0, 
    228, 128,   4,   0,   0,   4, 
      0,   0,   7, 128,  10,   0, 
    228, 160,   1,   0, 170, 144, 
      0,   0, 228, 128,   1,   0, 
      0,   2,   1,   0,   1, 128, 
      5,   0,   0, 160,  12,   0, 
      0,   3,   1,   0,   1, 128, 
      1,   0,   0, 128,  13,   0, 
    255, 160,   2,   0,   0,   3, 
      2,   0,  15, 128,   3,   0, 
    228, 144,   5,   0, 149, 160, 
      4,   0,   0,   4,   1,   0, 
     15, 128,   1,   0,   0, 128, 
      2,   0, 228, 128,   5,   0, 
    192, 160,   1,   0,   0,   2, 
      2,   0,   1, 128,  13,   0, 
      0, 160,   4,   0,   0,   4, 
      2,   0,   7, 128,  15,   0, 
    228, 160,   2,   0,   0, 128, 
      1,   0, 228, 128,   1,   0, 
      0,   2,   1,   0,   7, 128, 
      2,   0, 228, 128,   1,   0, 
      0,   2,   2,   0,   8, 128, 
      5,   0,   0, 160,  38,   0, 
      0,   1,   0,   0, 228, 240, 
      2,   0,   0,   3,   3,   0, 
      1, 128,   2,   0, 255, 128, 
     16,   0,   0, 160,   5,   0, 
      0,   3,   3,   0,   1, 128, 
      3,   0,   0, 128,   4,   0, 
      0, 160,  46,   0,   0,   2, 
      0,   0,   1, 176,   3,   0, 
      0, 128,   8,   0,   0,   4, 
      3,   0,   1, 128,   0,   0, 
    228, 128,  19,  32, 228, 161, 
      0,   0,   0, 176,  11,   0, 
      0,   3,   3,   0,   1, 128, 
      3,   0,   0, 128,   5,   0, 
      0, 160,   5,   0,   0,   4, 
      3,   0,   7, 128,   3,   0, 
      0, 128,  17,  32, 228, 160, 
      0,   0,   0, 176,   4,   0, 
      0,   4,   1,   0,   7, 128, 
      3,   0, 228, 128,  13,   0, 
    170, 160,   1,   0, 228, 128, 
      2,   0,   0,   3,   2,   0, 
      8, 128,   2,   0, 255, 128, 
      5,   0, 255, 160,  39,   0, 
      0,   0,  11,   0,   0,   3, 
      1,   0,  15, 128,   1,   0, 
    228, 128,   5,   0,   0, 160, 
     10,   0,   0,   3,   1,   0, 
     15, 128,   1,   0, 228, 128, 
      5,   0, 255, 160,   5,   0, 
      0,   3,   0,   0,  15, 208, 
      1,   0, 228, 128,  12,   0, 
    228, 160,   2,   0,   0,   3, 
      0,   0,   1, 128,   0,   0, 
    255, 128,  14,   0,  85, 161, 
      5,   0,   0,   3,   0,   0, 
      1, 128,   0,   0,   0, 128, 
     14,   0, 170, 160,  11,   0, 
      0,   3,   0,   0,   1, 128, 
      0,   0,   0, 128,  14,   0, 
    255, 160,  10,   0,   0,   3, 
      0,   0,   4, 224,   0,   0, 
      0, 128,   5,   0, 255, 160, 
      1,   0,   0,   2,   0,   0, 
      3, 224,   2,   0, 228, 144, 
    255, 255,   0,   0
};