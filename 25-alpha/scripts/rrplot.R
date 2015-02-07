# don't use this file, it was only used for the August 8 SibertPTTP-Notes.tex
# see MDtab.R for current version
#
require("movemodutils")

rr.plot<-function(fit="skj27.p02")
{
# op <- par(no.readonly = TRUE) # the whole list of settable par's.
# print(par("xpd"))
# par(xpd=FALSE)
# print(par("xpd"))

  total.caps<-c(23, 183, 1009, 1039, 3, 154, 95, 3055, 859, 2784, 187, 314, 1026)
  stc<-sort(total.caps, index.return=TRUE)
  ndx<-stc$ix
  admb<-read.admb.files(fit)
  par <- read.par(fit)
  rr<-which(admb$names == "report_rates")
  std<-2*admb$std[rr]
  std<-c(std[ndx],0.12,0.18)
  est<- admb$est[rr]
  est<-c(est[ndx],0.53,0.45)
  names <- c(toupper(substr(par$fleet.names[ndx],1,2)),"M2","M3")
  #yrange<-range(est-std,est+std)
  yrange <- c(min(est-std),1.2*max(est+std))

  x11(width=9.0,height=4.5)
  plot(c(1,length(names)),yrange,type='n',axes=FALSE,ann=FALSE)
  points(est,pch=5,cex=1.6)
  points(est-std,pch='-',cex=1.6) #2)
  points(est+std,pch='-',cex=1.6) #6)
# axis(1,at=c(0,length(names)+1))
  axis(1,at=c(par("usr")[1],par("usr")[2]),labels=FALSE, lwd.tick=0)
  axis(1,at=seq(1,length(names)),labels=names,cex=0.6)
  mtext("Purse Seine Fleet and MFCL Regions",side=1,line=3,cex=1.2)

  axis(2,at=c(par("usr")[3],par("usr")[4]),labels=FALSE, lwd.tick=0)
  axis(2,las=1)
  mtext("Reporting Rate",side=2,line=3,cex=1.2)
  for (n in 1:length(names))
    lines(c(n,n),c(par("usr")[3],est[n]+std[n]),col="gray",lty="dotted")
 
  abline(h=est[15]-std[15],col="gray")
  abline(h=est[14]+std[14],col="gray")

  save.png.plot(paste("rr-",fit,sep=""),width=9.0,height=4.5)
# par(op)
}

save.png.plot<-function(root,width=6.5,height=4.5)
{
# graphics.root <-paste("./graphics/",root,sep="")
  graphics.root <-paste("./",root,sep="")
  file.png <-paste(graphics.root,".png",sep="")
  file.eps <-paste(graphics.root,".eps",sep="")
  file.pdf <-paste(graphics.root,".pdf",sep="")
  dev.copy2pdf(file=file.pdf,width=width,height=height)
# dev.copy2eps(file=file.eps,width=width,height=height)
  cmd <- paste("convert -antialias -density 300",file.pdf,file.png,sep=" ")
  system(cmd)
  print(paste("Plot saved as ",file.pdf," and converted to ", file.png,sep=""),
              quote=FALSE)
# system(paste("rm -fv",file.pdf))
}

# from tagmove.log
#Total recaptres by fleet:
#  1    cnps   17    6   23
#  2    fmps   22  161  183
#  3    idps  110  899 1009
#  4    jpps   33 1006 1039
#  5    kips    2    1    3
#  6    krps   23  131  154
#  7    mhps    1   94   95
#  8    pgps   72 2983 3055
#  9    phps  224  635  859
# 10    sbps  267 2517 2784
# 11    twps   48  139  187
# 12    usps   73  241  314
# 13    vups  778  248 1026



