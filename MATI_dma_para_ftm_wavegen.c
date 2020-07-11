#include "DMA.h"

#define DMA_WAVEGEN_CH 0    //esto va a ir en el DMA.h
#define DMA_FTM1_CH0 28     //esto va a ir en el DMA.h


//memory to peripheral ejemplo
dma_transfer_conf_t conf;

configureDMAMUX(DMA_WAVEGEN_CH, DMA_FTM1_CH0, false);
conf.source_address = (uint32_t)arreglo_con_valores;
conf.dest_address = (uint32_t)&registro;
conf.offset = 0x02;
conf.transf_size = BITS_16;
conf.bytes_per_request = 0x02;	//paso 16bits=2bytes en cada dma request
conf.total_bytes = conf.bytes_per_request*16;	//el total será 2bytes*16
conf.mode = MEM_2_PERIPHERAL;
DMAPrepareTransfer(DMA_WAVEGEN_CH, &conf);
lkeuwfn-0 mp[mi u
90wqFX  PHQeinuf0 \pou2gnq[e97fyu \
0p[owMEFY98CYU  
M-0wpu9v8 up0woeFNX8U EUW\p0of;xpy8 3rewyd]u0fp[xw89easdvy]0- [pxi2mr3qeuxafn8[90 m2\n48TDFX09KP§I2MU ]90D4NCF8 0X2PR[FUX W0p\iqpfejh[t2-
                                                                                                                                      0Q{IEOWFH 0]9 ]29R  IM0POny[r0x82 p4BNR2C0§JK
                                                                                                                                      D-M0  [2IMU
                                                                                                                                             NR90Xuw]09rimct
                                                                                                                                             02 -ucy]90r8m
                                                                                                                                        § 2[0piurf]n09  x20ctun2039qrkx
                                                                                                                                        -0, p4my2n98werndx
                                                                                                                                        -0§2  iur09pui
                                                                                                                                        -xio2uyNR90FIU4 
                                                                                                                                        -K02RUSM90U 
                                                                                                                                        -02pour3dy
                                                                                                                                        2-=0  [purn89x§u2 
                                                                                                                                               0poruynx89 2r8
                                                                                                                                               -0[pou ym24ru
                                                                                                                                                  -0  §2umyr9n8m
                                                                                                                                                  x§20  -umrn89y2 4je
                                                                                                                                                  m0-r  y23e
                                                                                                                                                  m-0§uyr2u0p§2ru38cj
                                                                                                                                                  -§98  0-
                                                                                                                                                  [crn9]02  0
                                                                                                                                                  -r[p\un y[982wuyR0-P  IMUNED0I§MS
                                                                                                                                                  09P O2DRM
                                                                                                                                                  -0§2PO  IUNR89E]09DPOSI§2M0 PRYN890 2M
                                                                                                                                                     0\pry[n983ru
                                                                                                                                                     2-0  por3n89n  2
                                                                                                                                                     -0pumy[98ryej]0  piut4m]089x2  y]
                                                                                                                                                  -0UYNGQ7ERYF]0-
                                                                                                                                               §  X9I2RM89qweys09opfum  i24tyew
                                                                                                                                               ium90]38 4tyn2[0WPOUMTOI 24EFN89JC8
                                                                                                                                               KM 0P24Omhyw[n9ysr]m09p  odkh2inuwry09]p iu2m9iouGNTW89[FUK
                                                                                                                                               -0S  [PR2IUDE789R]M-09PO MU2RY0]9FP3 UM13OTIN4WTYREF[98O UF,P9OMDNY98[ 2J]0-PORIUM]N2  RY3098I
                                                                                                                                          M\P2RPT87wey7]rj0-9mu yn[2ty2]-0umdn2 yr]09u§ 2[ywerf9i1
                                                                                                                                          0n9]iot34 yw98E0UR
                                                                                                                                        M0-§2DPOU4  TYFU
                                                                                                                                        M§-02 PUMDUMR
                                                                                                                                        0-MU89  R
                                                                                                                                        -0POU2MYH YR0]9PSOMU2DY N[98RUM ]92PRMIP  O2MURY89JY7CU
                                                                                                                                        -03 [PW489-7Q3RTY[0G[WPRY 2U894E=901NR3U2
                                                                                                                                        0-39RP1OIXQH891MU4  \W98UETYRX-0§[I MU2YQ]EN-0[PRIOMS1DN][38  4YTUERM[PE32  89TYGN
                                                                                                                                        0-1P[O3MU4N 2YN-0\POTDM1[N9 IU23R0PD§I9 2YWR90[ENU§\31P 9uy]908wepury4rc-]0§i 2]n98eYWF0]9POUM2]R IONyrwef089ewimr§oditun 0]48im
                                                                                                                                        -x095y[ 903
                                                                                                                                               m0ix9tngb237r9]0nm2§ 0-18m43nsyr8[9n4op3§im  0[13xy]90,ty]1n9[8d3  try
                                                                                                                                               -0pxo  i2mur09p24,i3ryxng[93runmr0
                                                                                                                                               pc[§2  nu[89R08[MO2§DUP0O9 2U9Nc]890uyqp0orum]2  ciuybr0]9xe3muc-29t uy]N0Q9POCJR][JK4GEFY]-04P  2§OTIUN82nr09im
                                                                                                                                               p24otucy 2-
                                                                                                                                               =[PTIU2unr90]fpomucoitn890u0rcm\p  GT789UR-0
                                                                                                                                               CI
                                                                                                                                               UM2  0-tu8yx]90iomosl4i2ct
                                                                                                                                             0  poitn89riug 0poxhGIUWEYFU]0N9-XPO M0-8iu
                                                                                                                                        ]-20po4unot8cnu]90rfm]  ioxuewfgc[98iug mp9otigwruicdyu]f90u  ]09cWEY]9CKFQWEFHC9N8
                                                                                                                                        U -0PUy]n[98yu]rx0-\px2o  ujrhtiuwey09\p  couitn]09o0POMU CH3I[Utwnrf90u  
                                                                                                                                        \npc98xum]0p9 mt4unwe98ycn]-0 utn[iu  8wyeoprmc ]3[iouty[W09OIUCT4  4G2W87RYN\20  OPUTH[897ry90pciornu  980c34
