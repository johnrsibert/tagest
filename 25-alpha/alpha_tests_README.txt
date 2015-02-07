/home/jsibert/Projects/SPC-Tagging/SPC2012/pttp/OneDegree/nominal/Grid23/alpha

Results of natural mortality dev tests with different weights.
All fits converged.
fit  weight  M       L         n   source dir
p002  0.0   0.147  22349.77    77  25
p003  0.0   0.147  22349.77    77  25-alpha
p004 -0.1   0.177  22065.26   105  25-alpha
p005 -1.0   0.192  22065.33   105  25-alpha
p006 -10.0  0.203  22112.79   105  25-alpha
p007 -0.1   0.177  22065.26   105  25-alpha
p008 -0.01  0.173  22064.14   105  25-alpha

Statisitcal improvement in fit compared to no devs, but M increases.

Similar results with fishing mortality devs

Tested something like reporting rate deves in prd_cap2.cpp using
unconstrained (ie do not sum to 0) devs. E_{xy} = 0, F_{xy} = 0,
pred_{xy} = 0. Therefore L_{xy} = 0 in Poisson likelihood computation. 
cf  if (tpred > 0.0) in Poisson likelihood computation.

