pretty<-TRUE
mm.NB<-function(tpred, tobs=5, w=1)
{
  latp = log(tpred + w)
  tobsa = tobs + w

  t1 <- mm_gammln(tobsa) - mm_gammln(w) - mm_gammln(tobs+1.0)
  t2 <- w*(log(w) - latp) + tobs*(log(tpred+1e-10)-latp)

  NB <- t1+t2
  return(NB)
}
#from nblike2.cpp
# DOUBLE a = 0;
# if (nb_scale)
# a = w*tpred;
# else
# a = w;
# DOUBLE latp = log(tpred + a);
# DOUBLE tobsa = tobs + a;
# temp += mm_gammln(tobsa) - mm_gammln(a) - mm_gammln(tobs+1.0);
# temp += a*(log(a) - latp) + tobs*(log(tpred)-latp);

mm_gammln<-function(x)
{
  return(lgamma(x))
}


NB.plot<-function()
{
   tpred <- seq(0,20,0.1)
   ntp = length(tpred)  
   w<-c(0.1,0.5,1.0,2.5,5)
   ncurve<-length(w)
   yrange<-c(0,.375)
   xrange<-c(0,(max(tpred)+2))

   plot(xrange,yrange,type='n',axes=FALSE,xlab="Predicted",ylab="Probability")
   axis(1,lwd=0,lwd.ticks=1)
   abline(h=par("usr")[3],lwd=1)
   axis(2,lwd=0,las=1,lwd.ticks=1)
   abline(v=par("usr")[1],lwd=1)

   xleg=1.025*max(tpred)

   tobs<-c(1,10)
   fcol<-c("lightblue","orange")
   bcol<-c("blue","red")
   up <- c(0.5,1.0)

   for (t in 1:length(tobs))
   {
     P.prob <- dpois(tobs[t],tpred)
     double.lines(tpred,P.prob,fcol=fcol[t],bcol=bcol[t],lwd=7)

     for (n in 1:ncurve)
     {
        NB.prob<-exp(mm.NB(tpred=tpred,tobs=tobs[t],w=w[n]))
        lines(tpred,NB.prob,col=bcol[t],lwd=2)
        yleg <- max(NB.prob)
        lines(c(tobs[t],max(tpred)),c(yleg,yleg),col=bcol[t],lty="dotted")
        text(xleg,yleg,lab=paste(w[n]),adj=c(0.0,up[t]),col=bcol[t])
     #  text(xleg,NB.prob[ntp],lab=paste(w[n]),adj=c(0.0,up[t]),col=bcol[t])
     }
     abline(v=tobs[t],col=bcol[t],lwd=2)
   }
   save.png.plot("negative-binomial")
}

double.lines<-function(x,y,bcol="black",fcol,lwd)
{
  if (pretty)
  {
    lines(x,y,col=bcol,lwd=lwd+2)
    lines(x,y,col=fcol,lwd=lwd)
  }
  else
    lines(x,y,col=fcol,lwd=2)
}

save.png.plot<-function(root,width=6.5,height=4.5,gpath="./graphics/")
{
  graphics.root <-paste("/home/jsibert/movemod/SPC2011/Reports/graphics/",root,sep="")
  file.png <-paste(graphics.root,".png",sep="")
  file.pdf <-paste(graphics.root,".pdf",sep="")
  dev.copy2pdf(file=file.pdf,width=width,height=height)
  cmd <- paste("convert -antialias -density 300",file.pdf,file.png,sep=" ")
  system(cmd)
  print(paste("Plot saved as ",file.pdf," and converted to ", file.png,sep=""),
              quote=FALSE)
}

