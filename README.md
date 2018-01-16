# The tagest-tagmove software suite for spatially resolved analysis fish tag data

## System requirements
* linux operating system on 64bit architecture (Previous versions of
  this software were perfectly functional on 32bit architecture under
  both linux and Windows running cygwin, so a port to Windows is
  feasible. But why bother?)
* [g++ compiler](https://gcc.gnu.org/) version 4.8 or greater.
* A current [ADMB implementation](http://admb-project.org/).
* Java JDK 7, necessary to build the jnigraphics library used by
  visualization tool such as tagmove.
* Some version of [GMT - The Generic Mapping
  Tools](http://gmt.soest.hawaii.edu) is useful for mapping. There are
  several scripts that invoke parts of GMT for mapping model domains,
for instance. (Some incompatibilities between GMT versions 5.1.x and
4.5.x have been recently noted. Current scripts were developed for
4.5, but some have been ported to 5.1.2.)

## Directory Structure
Some of this code has been under development since 1989, long before the
widespread adoption of version control systems. An *ad hoc* directory
numbering system was used to keep separate versions. The files here on
github are an export of revision 3211 from the PFRP Subversion
server, honmaguro.soest.hawaii.edu, at the University of Hawaii at
Manoa.

Each directory has subdirectories for C++ source code (src), ADMB
template files (tpl), documentation (docs), and various shell scripts,
awk code and R code (scripts). Often these scripts are specific to the
version in the parent directory.

Most directories and subdirectories have a Makefile, so it is usually
only necessary to run make.

A provisional "work in progress" user manual is available at 
25/docs/manuals/TagestUserMan.pdf.

#### 25
The current "production" version, developed largely to analyze 
tagging data from the current SPC tagging project (PTTP).
This code is a conversion of the code in directory 21 into an ADMB application
using template classes to implement different code for constant (eg
double) and variable (eg dvariable) objects. Eun Jung Kim's models of
FAD attraction and stickiness are implemted here.

#### 25mt
Multithread example using OpenMPI

#### 25-alpha
Sibert's attempt to create satisfactory alternatives for handling errors
in reporting effort, missing effort and errors in reporting recaptures.
A dead end.

#### 26
Revision of the directory 25 code without template classes and the
nerural network classes. Only the movemod simulator as been tested. It
implemented using variable types and the AUTODIF syntax
`gradient_structure::set_NO_DERIVATIVES();`

#### 21
Code base with user written adjoint code, highly optimized to run on
slower, memory deficient computers. At one time, it could be compiled
under both both windows and linux. It is not compliant to current C++
standards. This code produced the results in Sibert and Hampton (2003).

#### 21mt
Multihread example using pthreads and thread pool to implement threading of the adi algorithm and its adjoint.

#### jnigraphics
Java native mode interface graphics to implement the tagmove
visualizations. Developed and maintained by Johnoel Ancheta.

#### example
A worked example using data from the Skipjack Survey and Assessment
Programme conducted by the South Pacific Commission between 1977 and 1987. 
The national identities in the tag return and fishing effort
records have been obfuscated out of ``confidentiality'' concerns.

To build a functional run directory, type make at the command prompt.

## Contributors
This project began in about 1989. Over the years,
a number of collaborators have contributed code and ideas.

* John Sibert
* Dave Fournier
* John Hampton
* Johnoel Ancheta
* Eun Jung Kim
* M. Shiham Adam
* Peter Bills
* Richard Bailey
* Pierre Kleiber

## Financial Support
* Food and Agricultural Orangization of the United Nations
* Secretariat of the Pacific Commnity
* Pelagic Fisheries Research Program, University of Hawaii at Manoa

## Publications
Sibert, J.R., Hampton, J., Fournier, D., 1996. Skipjack movement and
fisheries interaction in the Western Pacific. Second FAO Expert
Consultation on Interactions of Pacific Tuna Fisheries, Shimizu,
Japan.

Bills, Peter J. and John R. Sibert, 1997. Design of tag-recapture
experiments for estimating yellowfin tuna stock dynamics, mortality,
and fishery interactions. SOEST Publication 97-05, JIMAR Contribution
97-313, 80 pp.

Sibert, J.R., Hampton, J., Fournier, D.A., Bills, P.J., 1999. An
advection-diffusion-reaction model for the estimation of fish movement
parameters from tagging data, with application to skipjack tuna
(Katsuwonus pelamis). Canadian Journal of Fisheries and Aquatic
Sciences, 56: 925-938.

Adam, M.S., Sibert J., 2002. Population dynamics and movements of
skipjack tuna (Katsuwonus pelamis) in the Maldivian fishery: analysis
of tagging data from an advection-diffusion-reaction model. Aquat.
Living Resour. 15: 13-23.

Sibert, J., Hampton, J., 2003. Mobility of tropical tunas and the
implications for fisheries management. Marine Policy 27 (2003) 87-05.

Adam, M. Shiham and John R. Sibert, 2004. Use of neural networks with
advection-diffusion-reaction models to estimate large-scale movements
of skipjack tuna from tagging data. SOEST Publication 04-03, JIMAR
Contribution 04-350,31 pp.
