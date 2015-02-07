<center>tagest</center>
======

### Movemod: The tagest-tagmove software suite for spatially resolved analysis fish tag data

###Directories

####25
The current "production" version, developed largely to analyze 
tagging data from the current SPC tagging project (PTTP).
This code is a conversion of the code in directory 21 into an ADMB application
using template classes to implement different code for constant (eg
double) and variable (eg dvariable) objects. Eun Jung Kim's models of
FAD attraction and stickiness are implemted here.

####25-alpha
Sibert's attempt to create satisfactory alternatives for handling errors
in reporting effort, missing effort and errors in reporting recaptures.
A dead end.

####26
Revision of the directory 25 code without template classes and the
nerural network version.
The movemod simulator works with all variable types using the 
AUTODIF syntax 
```C++
  gradient_structure::set_NO_DERIVATIVES();
```

####21
Code base with user written adjoint code, highly optimized to run on
slow-memory deficient computers. At one time, it could be compiled
under bothe Cygwin and linux. The code is not compliant to current C++
standards. The code produce the results in Sibert and Hampton (2003).

####jnigraphics
Java native mode interface graphics to implement the tagmove
visualizations.

###Contributors
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

###Financial Support
* Food and Agricultural Orangization of the United Nations
* Secretariat of the Pacific Commnity
* Pelagic Fisheries Research Program, University of Hawaii at Manoa

###Publications
Sibert, J.R., Hampton, J., Fournier, D., 1996. Skipjack movement and
fisheries interaction in the Western Pacific. Second FAO Expert
Consultation on Interactions of Pacific Tuna Fisheries, Shimizu,
Japan.

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
