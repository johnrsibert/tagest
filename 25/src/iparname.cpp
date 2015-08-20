//$Id: iparname.cpp 3163 2014-04-26 01:58:02Z eunjung $
#include "ipar.h"

void set_ipar_names(ipar_t& t)
{
  t.set_name(1, "Maximum number of function evaluations");
    t[1] = 1000;
  t.set_name(2, "Iterations between minimizer displays");
    t[2] = 1;
  t.set_name(3, "Convergence criterion * 1000");
    t[3] = 100;
  t.set_name(4, "Maximum number of iterations");
    t[4] = 30;
  t.set_name(5, "Number of calls made to fgcomp during fit");
    t[5] = 0;
  t.set_name(6, "if >0, number of time steps per month");
    t[6] = 4;
  t.set_name(7, "fmm.scroll_flag; 1 => scroll ALL variables; 0 => overwrite screen");
    t[7] = 0;
  t.set_name(8, "fit number (usually decimal equivalent to the hex file name extention number)");
  t.set_name(9, "total elapsed minutes to do fit)");
  t.set_name(10, "fmm.ireturn");
  t.set_name(11, "likelihood function: 0=> testlk; 1=> robust_likelihood; "
                 "2=> poisslk; 3=> least_square; "
                 "4,14,24=> negative binomial; 5,15,25=> negative binomial const.; "
                 "6=> exponential");   // 4,14 24:  24 is the ipar value.. 4 is for min and max and 24 is for initial values.. one for each fleet in the model
    t[11] = 2;
  t.set_name(13, "tag cohort pooling; 0 => no pooling; 1=> pooling; 2 => pooling by date");
    t[13] = 1;
  t.set_name(14, "0 => omit month at liberty n from from likelihood (resurrected, but still bogus)");
  t.set_name(16, "global average effort for simultor to generate effort files");
    t[16] = 0;
  t.set_name(17, "global effort SD for simultor to generate effort files");
    t[17] = 0;
  t.set_name(18, "1=> scale NB parameter by predicted recaptures");
    t[18] = 0;
  t.set_name(19, "0 => no special M for any region (default); otherwise region number for special motality estimation");
    t[19] = 0;
  t.set_name(20, "1=> use Bills corrected total mortality");
  t.set_name(21, "sigma active; 1=> uniform sigma; 2=> separate sigma for each region");
    t[21] = 2;
  t.set_name(22, "natural mortality active");
    t[22] = 1;
  t.set_name(23, "1 => (u,v) active");
    t[23] = 1;
  t.set_name(24, "number of fishing mortality anomalies; deprecated");
    t[24] = 0;
  t.set_name(25, "q active; 1=> uniform q; 2=> separate q for each fleet");
    t[25] = 2;
  t.set_name(26, "No longer used!");
//  t.set_name(26, "!=0 => regional paramters (u,v,sigma) only active for regions with tag returns");
//    t[26] = 0;
   t.set_name(27, "catch equation for tag recapture: 0=> N*f; 1=> N*f*(1-exp(-z))/z (Baranov)");
    t[27] = 0;
  t.set_name(28, "reporting rate active");
    t[28] = 0;
  t.set_name(29, "total tag penalty *1e8");
  t.set_name(30, "-log_10 tag scaling factor; 0 => no scaling");
  t.set_name(31, "x component curvature penalty * 1e9");
    t[31] = 30;
  t.set_name(32, "y component curvature penalty * 1e9");
    t[32] = 30;
  t.set_name(33, "velocity curvature penalties in effect");
    t[33] = 1;
  t.set_name(34, "near zero velocity smoothing scale * 100");
    t[34] = 5;
  t.set_name(35, "number of points in velocity moving average smoothing function");
    t[35] = 3;
  t.set_name(36, "number of points in diffusion moving average smoothing function");
    t[36] = 0;
  t.set_name(37, "100 * constant smimming speed");
  t.set_name(38, "tau and lambda amplification factor for random walk simulator");
  t.set_name(39, "effort standard deviation for simulator");
  t.set_name(40, "random number seed for simulator");
  t.set_name(41, "sigma curvature penalties in effect");
    t[41] = 1;
  t.set_name(42, "sigma curvature penalty * 1e9");
    t[42] = 30;
  #ifdef OLD_DO_KLUDGE_
  t.set_name(43, "set PHDO effort to zero if no recaptures");
  t.set_name(44, "set IDDO effort to zero if no recaptures");
  #endif
  t.set_name(45, "use monthly average effort; no longer used?");
  t.set_name(46, "bin width for displacement calculations; 0 => 60 NMi");
    t[46] = 60;

  t.set_name(47, "non-zero=>saves the non-zero viewport as a gif(pcx) file");
  t.set_name(48, "1=>write tags as real numbers");
  t.set_name(49, "1=> use centered scheme; 0=> use upwind scheme");
    t[49] = 0;
  t.set_name(50, "0=> use 60 in all of deltax; 1=> create spherical coords");

  t.set_name(51, "1 => normalize fishing effort to fleet means");
    t[51] = 1;
  t.set_name(52, "DO NOT EDIT; value set automagically (1 => catchabilities have been adjusted to normalized effort)");
  t.set_name(53, "include effortless recaps in Z: 0 => omit; 1 => f = inverse catch equation; 2 => f= q\\bar{E}");
  t.set_name(54, "reserved for possible use in effortless recap handling");

  t.set_name(56, "southwest longitude of recruitment area; 0 => model area");
  t.set_name(57, "southwest latitude of recruitment area; 0 => model area");
  t.set_name(58, "northeast longitude of recruitment area; 0 => model area");
  t.set_name(59, "northeast latitude of recruitment area; 0 => model area");
  t.set_name(60, "total recruitment in recruitment area; 0 => ");


  t.set_name(61, "population maximum for history plot");
  t.set_name(62, "catch maximum for history plot");
  t.set_name(63, "maximum for population for color scale");
  t.set_name(64, "0=>display matrix only; 1=>display the coastline of the region and matrix; 2=>display coastline only");
  t.set_name(65, "0=> continuous run; 1=> use keyboard for next frame");
  t.set_name(66, "0=>use no scaling for uvd; 1=> use scales from scale.dat for uvd read in by read_seasons");
  t.set_name(67, "1=> save the no. of releases /month /gridsquare");
  t.set_name(68, "1=> graphics on (default); 0=> graphics off");
    t[68] = 1;
  t.set_name(69, "1=> reads in seasonal info from year_XX.uvd files");
  t.set_name(70, "movement pattern display mode: 0=> circles and arrows; 1=> color map and arrows");


  t.set_name(71, "u_slope: 1 => active, 0 => inactive");
  t.set_name(72, "u_offset:  -- ditto--");
  t.set_name(73, "v_slope: -- ditto--");
  t.set_name(74, "v_offset: -- ditto--");
  t.set_name(75, "sigma_slope: -- ditto--");
  t.set_name(76, "sigma_offset: -- ditto--");
  t.set_name(77, "1=> D^2 before scaling; 2=> D^2 after scaling");
  t.set_name(78, "NN weight penalty in effect; 1=> penalize first layer only; 2=> only penalize hidden layer");
  t.set_name(79, "Weight penalty*1.e-9");
  t.set_name(80, "0=> apply scaling function to output layer; 1=> no scaling");

  t.set_name(81, "1 => switch_penalty in effect;  0=> not in effect" );
  t.set_name(82, "Switch_penalty *1.e-9");
  t.set_name(83, "FAD factor model code; 0=>do not read; 1=>read FADmap"); 
  t.set_name(84, "FAD-uv factor model code; 0=>no effect; 1=>effect by uv computation"); 
  t.set_name(85, "choice of FAD-D model code; 0=>no effect; 1=>FAD-D logistic model; 2=>FAD-D exponential model");
  t.set_name(86, "FAD-uv factor model code-using chi; -1=>no effect;1=>effect by uv computation");
  t.set_name(87, "FAD-D exponential model code-using gamma; -1=>no effect;1=>effect by sigma computation");
  t.set_name(88, "FAD-D logistic model code-using logslope; -1=>no effect;1=>effect by sigma computation");
  t.set_name(89, "FAD-D logistic model code-using inflection; -1=>no effect;1=>effect by sigma computation");
  t.set_name(90, "FAD-D logistic model code-using min_D: -1=>no effect;1=>effect by sigma computation");
  
  //t.set_name(91, "FAD-D exponential model code;1=>efect by sigma computation");

}
