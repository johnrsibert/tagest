> outer(0:5,0:5,function(x,y)sqrt(x^2+y^2))
     [,1]     [,2]     [,3]     [,4]     [,5]     [,6]
[1,]    0 1.000000 2.000000 3.000000 4.000000 5.000000
[2,]    1 1.414214 2.236068 3.162278 4.123106 5.099020
[3,]    2 2.236068 2.828427 3.605551 4.472136 5.385165
[4,]    3 3.162278 3.605551 4.242641 5.000000 5.830952
[5,]    4 4.123106 4.472136 5.000000 5.656854 6.403124
[6,]    5 5.099020 5.385165 5.830952 6.403124 7.071068
> junk <- sort(unique(60*as.vector(outer(0:5,0:5,function(x,y)sqrt(x^2+y^2)))))
> junk
 [1]   0.00000  60.00000  84.85281 120.00000 134.16408 169.70563 180.00000
 [8] 189.73666 216.33308 240.00000 247.38634 254.55844 268.32816 300.00000
[15] 305.94117 323.10989 339.41125 349.85711 384.18745 424.26407
> .5*(junk[-1]+junk[-length(junk)])
 [1]  30.0000  72.4264 102.4264 127.0820 151.9349 174.8528 184.8683 203.0349
 [9] 228.1665 243.6932 250.9724 261.4433 284.1641 302.9706 314.5255 331.2606
[17] 344.6342 367.0223 404.2258
