#include "ipar.h"

// should change to member functions of some par_t class, e.g.
// void par_t_reg::set_ipar_names(ipar_t& t)

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
                 "6=> exponential");
    t[11] = 2;
  t.set_name(13, "tag cohort pooling; 0 => no pooling; 1=> pooling; 2 => pooling by date");
    t[13] = 1;
  t.set_name(14, "0 => omit n months from release from likelihood");
    t[14] = 0;
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
  t.set_name(24, "number of fishing mortality anomalies");
    t[24] = 0;
  t.set_name(25, "q active; 1=> uniform q; 2=> separate q for each fleet");
    t[25] = 2;
  t.set_name(26, "No longer used!");
//  t.set_name(26, "!=0 => regional paramters (u,v,sigma) only active for regions with tag returns");
//    t[26] = 0;
   t.set_name(27, "tag recapture computation: 0=> N*f; 1=> N*f*(1-exp(-z))/z");
    t[27] = 1;
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
  t.set_name(43, "set PHDO effort to zero if no recaptures");
  t.set_name(44, "set IDDO effort to zero if no recaptures");
  t.set_name(45, "use monthly average effort");
  t.set_name(46, "bin width for displacement calculations; 0 => 60 NMi");
    t[46] = 60;
  /*
  t.set_name(51, "index of target fishery in popsim");
  t.set_name(52, "index of growing fishery in popsim");
  t.set_name(53, "number of growth steps for growing fishery in popsim; 0 => 10 steps");
  t.set_name(54, "equillibration time for growing fishery in popsim; 0 => 24 months");
  t.set_name(55, "expasion factor");
  t.set_name(56, "1 => specific area for growing fishery to expand");
  t.set_name(57, "southwest longitude of expansion area");
  t.set_name(58, "southwest latitude of expansion area");
  t.set_name(59, "northeast longitude of expansion area");
  t.set_name(60, "northeast latitude of expansion area");
  */
  t.set_name(61, "population maximum for history plot");
  t.set_name(62, "catch maximum for history plot");
  t.set_name(63, "maximum for population for color scale");
  t.set_name(64, "0=>display matrix only; 1=>display the coastline of the region and matrix; 2=>display coastline only");
  t.set_name(65, "0=> continuous run; 1=> use keyboard for next frame");
  t.set_name(66, "0=>use no scaling for uvd; 1=> use scales from scale.dat for uvd read in by read_seasons");
  t.set_name(67, "1=> save the no. of releases /month /gridsquare");
  t.set_name(68, "1=> graphics on; 0=> graphics off");
  t.set_name(69, "1=> reads in seasonal info from year_XX.uvd files");

  t.set_name(70, "movement pattern display mode: 0=> circles & arrows; 1=> color map and arrows");
  
  //switches for neural_net
  t.set_name(71, "Squashing functions: 0 => None; Sigmoid => 1; Linear => 2" );
  t.set_name(72, "Square output_layer(3) before scaling"); 
  
  /*
  t.set_name(71, "fishery n growth mode for reset_q");
  t.set_name(72, "fishery n growth mode for reset_q");
  t.set_name(73, "fishery n growth mode for reset_q");
  t.set_name(74, "fishery n growth mode for reset_q");
  t.set_name(75, "fishery n growth mode for reset_q");
  t.set_name(76, "fishery n growth mode for reset_q");
  t.set_name(77, "fishery n growth mode for reset_q");
  t.set_name(78, "fishery n growth mode for reset_q");
  t.set_name(79, "fishery n growth mode for reset_q");
  t.set_name(80, "fishery n growth mode for reset_q");
  t.set_name(81, " .... ");
  */
  t.set_name(82, "non-zero=>saves the non-zero viewport as a gif(pcx) file");
  t.set_name(83, "1=>write tags as real numbers");
  t.set_name(84, "1=> use centered scheme; 0=> use upwind scheme");
  t.set_name(85, "0=> use 60 in all of deltax; 1=> create spherical coords");

  /*
  t.set_name(91, "recruitment computation mode for gcomp; 0 => g=1");
  t.set_name(92, "mean recruitment for gcomp mode 1");
  t.set_name(93, "recruitment SD for gcomp mode 1");
  */
  t.set_name(96, "u,v, sigma comutation mode; 0 => expand from gridmap");

  t.set_name(100, "read file equilb.pop (if present) in popsim");

  // switches for habtitat parameters
  t.set_name(101, "1=>Dmax active");
  t.set_name(102, "1=> dslope active");
  t.set_name(103, "1=> habitat_half active");

  t.set_name(104, "1=> aslope_x active");
  t.set_name(105, "1=> inflection_x active");
  t.set_name(106, "1=> velmax_x active");

  t.set_name(107, "1=> aslope_y active");
  t.set_name(108, "1=> inflection_y active");
  t.set_name(109, "1=> velmax_y active");

  t.set_name(110, "1=> a_one active");
  t.set_name(111, "1=> a_two active");
  t.set_name(112, "1=> a_three active");
  t.set_name(113, "1=> b_one active");
  t.set_name(114, "1=> b_two active");
  t.set_name(115, "1=> b_three active");
  t.set_name(116, "1=> c_one active");
  t.set_name(117, "1=> c_two active");
  t.set_name(118, "1=> c_three active");

  t.set_name(121, "1 => normalize fishing effort to fleet means");
  t.set_name(122, "DO NOT EDIT; value set automagically (1 => catchabilities have been adjusted to normaized effort)");

  t.set_name(126, "southwest longitude of recruitment area; 0 => model area");
  t.set_name(127, "southwest latitude of recruitment area; 0 => model area");
  t.set_name(128, "northeast longitude of recruitment area; 0 => model area");
  t.set_name(129, "northeast latitude of recruitment area; 0 => model area");
  t.set_name(130, "total recruitment in recruitment area; 0 => ");
  t.set_name(131, "recruitment computation mode; 0 => uniform recruitment; 3 => geographic box; 4 => temperature dependency");
  t.set_name(132, "target unfished eqillibrium population");
  t.set_name(133, "minimum SST for recruitment");
  t.set_name(134, "maximum SST for recruitment");
  /*
  t.set_name(91, "recruitment computation mode for gcomp; 0 => g=1");
  t.set_name(92, "mean recruitment for gcomp mode 1");
  t.set_name(93, "recruitment SD for gcomp mode 1");
  */
/*  
  //neural_net class switches, added  msa 1/7/2002 8:47PM
  t.set_name(141, "INIT WEIGHTS: 0 = > from par file, 1=> initialize  with 0.01; other =>  seed for random values");
  t.set_name(142, "IINIT BIAS: 0 = > from par file, 1=> initialize  with 0.0; other =>  seed for random values");
  t.set_name(143, "u_slope: 1=> active, 0 = inactive");
  t.set_name(144, "u_offset: 1=> active, 0 = inactive");
  t.set_name(145, "v_slope: 1=> active, 0 = inactive");
  t.set_name(146, "v_offset: 1=> active, 0 = inactive");
  t.set_name(147, "sigma_slope: 1=> active, 0 = inactive");
  t.set_name(148, "sigma_offset: 1=> active, 0 = inactive");
  t.set_name(149, "weights: 1=> active, 0 = inactive");
  t.set_name(150, "biases: 1=> active, 0 = inactive");
  t.set_name(151, "Year_flag:   1= active; 0  = inactive");
  t.set_name(152, "Month_flag");
  t.set_name(153, "Longitude_flag");
  t.set_name(154, "Latitude_flag");
  t.set_name(155, "Depth_flag");
  t.set_name(156, "Numfads_flag");
  t.set_name(157, "Elevation_flag");
  t.set_name(158, "Distance_flag");
  t.set_name(159, "Seasonal WStressCurl");
  t.set_name(160, "Spatio-Temporal WStressCurl");
  
  t.set_name(161, "SOI Index: 1=> active, 0 = inactive");
*/

}
