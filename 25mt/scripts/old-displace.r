rttpyft<-39354
rttpskj<-92351
ssap10<-94430
ssap16<-82874

dispread<-function(file)
{
   tailstring<-paste("tail -n +3 ",file, sep="")
   print(tailstring)
   options("warn"=-1)
   disp<-read.table(pipe(tailstring),sep=",",header=T,check.names=T)
   disp
}

#udispplot<-function(fit)
#{
#  file<-paste(fit,".prn",sep="")
#  disp<-dispread(file)
#  dispplot(disp,fit,"Displacement (NMi)")
#}

xdispplot<-function(fit)
{
  file<-paste(fit,".prn",sep="")
  disp<-dispread(file)
  dispplot(disp,fit,"E-W Displacement (NMi)")
}

ydispplot<-function(fit)
{
  file<-paste(fit,".prn",sep="")
  disp<-dispread(file)
  dispplot(disp,fit,"N-S Displacement (NMi)")
}

dispplot<-function(disp,fit,xtitle)
{
   require(modreg)
   x<-as.numeric(gsub("X","",gsub("X\\.","-",names(disp)[-1])))
#  x<-as.numeric(substring(names(disp)[-1],2))
#  print(x)
   wd<-getwd()
   mtitle<-paste(wd,"/",fit,sep="")
   plot(x,disp[1,-1],type='l',xlim<-c(-1000,1000), main= mtitle, 
        ylab="Number",xlab=xtitle)
   pts<-3
   fpts<-pts/length(x)
#  print(fpts)
   sdpr<-supsmu(x,as.numeric(as.vector(disp[1,-1])),span=fpts)
   lines(sdpr$x,sdpr$y,col=pts,lwd=2)
   lines(c(0,0),c(0,max(disp[1,-1])))
   lines(x,disp[3,-1])
   lines(x,disp[6,-1])
   lines(x,disp[12,-1])

   ps<-paste(fit,".ps",sep="")
   dev.copy(device=postscript, file=ps, horizontal=FALSE,
      paper="letter", height=6,width=6,pointsize=12)
   dev.off()
}

udisplot<-function(fit,releases)
{
  require(modreg)
  file<-paste(fit,".prn",sep="")
  disp<-dispread(file)
  dd<-disp[,-1]
  surv<-apply(dd,1,sum)
  deaths<- -1.0*diff(c(releases,surv))
  tt<-apply(dd,2,function(x)x*deaths/(surv+1e-8))
  x<-as.numeric(substring(names(dd),2))
  dpr<-apply(tt,2,sum)
  wd<-getwd()
  mtitle<-paste(wd,"/",fit,sep="")
  plot(x,dpr,type='l',xlim<-c(1,1000),lwd=2, main=mtitle,
       xlab="Net Displacement (NMi)",ylab="Number of Fish")
  points(x,rep(0,length(x)),pch=3)

  pts<-2
  fpts<-pts/length(x)
# print(fpts)
  sdpr<-supsmu(x,dpr,span=fpts)
  lines(sdpr$x,sdpr$y,col=pts,lwd=2)

  pts<-6
  fpts<-pts/length(x)
# print(fpts)
  sdpr<-supsmu(x,dpr,span=fpts)
  lines(sdpr$x,sdpr$y,col=pts)

  sdpr<-supsmu(x,dpr)
  lines(sdpr$x,sdpr$y)

  cdpr<-cumsum(dpr)/sum(dpr)
  cprmed<-approx(cdpr,x,.5)
  print(paste("Median displacement distance = ",cprmed$y," NMi",sep=""))
  lines(x,cdpr*max(dpr))
# lines(c(cprmed$y,cprmed$y),c(0,0.5*max(dpr)))
  arrows(cprmed$y,0.0,cprmed$y,0.5*max(dpr),code=1,angle=15,length=0.25,col="green")

  ps<-paste(fit,".ps",sep="")
  dev.copy(device=postscript, file=ps, horizontal=FALSE,
     paper="letter", height=6,width=6,pointsize=12)
  dev.off()
# wmf<-paste(fit,".wmf",sep="")
# dev.copy(device=win.metafile,filename = wmf, width = 6, height = 6, 
#          pointsize=12)
# dev.off()
# pngname<-paste(fit,".png",sep="")
# dev.copy(device=png,filename=pngname)
# dev.off()
}

udisplot2<-function(fit1,fit2,releases)
{
  require(modreg)
  file<-paste(fit1,".prn",sep="")
  disp<-dispread(file)
  dd<-disp[,-1]
  surv<-apply(dd,1,sum)
  deaths<- -1.0*diff(c(releases,surv))
  tt<-apply(dd,2,function(x)x*deaths/(surv+1e-8))
  x<-as.numeric(substring(names(dd),2))
  dpr<-apply(tt,2,sum)
  sdpr<-supsmu(x,dpr)
  lsty<-"solid"
# plot(sdpr$x,sdpr$y,type='l',xlim<-c(1,1000),lwd=2,lty=lsty,
#      xlab="Net Displacement (NMi)",ylab="Number of Fish")
  cdpr<-cumsum(dpr)/sum(dpr)
  cprmed<-approx(cdpr,x,.5)
  print(paste("Median displacement distance = ",cprmed$y," NMi",sep=""))
# lines(x,cdpr*max(dpr),lty=lsty)
  plot(x,cdpr*max(dpr),type='l',xlim<-c(1,1000),ylim<-c(0,3000),lwd=1,lty=lsty,
       xlab="Net Displacement (NMi)",ylab="Number of Fish")
  arrows(cprmed$y,0.0,cprmed$y,0.5*max(dpr),code=1,angle=15,length=0.25,lwd=1,lty=lsty)

  file<-paste(fit2,".prn",sep="")
  disp<-dispread(file)
  dd<-disp[,-1]
  surv<-apply(dd,1,sum)
  deaths<- -1.0*diff(c(releases,surv))
  tt<-apply(dd,2,function(x)x*deaths/(surv+1e-8))
  x<-as.numeric(substring(names(dd),2))
  dpr<-apply(tt,2,sum)
  sdpr<-supsmu(x,dpr)
  lsty<-"dashed"
# lines(sdpr$x,sdpr$y,lwd=2,lty=lsty)
  cdpr<-cumsum(dpr)/sum(dpr)
  cprmed<-approx(cdpr,x,.5)
  print(paste("Median displacement distance = ",cprmed$y," NMi",sep=""))
  lines(x,cdpr*max(dpr),lty=lsty)
  arrows(cprmed$y,0.0,cprmed$y,0.5*max(dpr),code=1,angle=15,length=0.25,lty=lsty)
  fit<-"displacements"
  ps<-paste(fit,".ps",sep="")
  dev.copy(device=postscript, file=ps, horizontal=FALSE,
     paper="letter", height=6,width=6,pointsize=12)
  dev.off()
# wmf<-paste(fit,".wmf",sep="")
# dev.copy(device=win.metafile,filename = wmf, width = 6, height = 6, 
#          pointsize=12)
# dev.off()
  pngname<-paste(fit,".png",sep="")
  dev.copy(device=png,filename=pngname)
  dev.off()
}

empdisplace<-function(file,title)
{
  recaps<-read.csv(file,header=T)
  print(paste(title, "; median = ",median(recaps$distnm),sep=""))
  mt<-paste(title, " (",median(recaps$distnm)," NMi)", sep="")
  fr<-range(0,1000)
  hist(recaps$distnm,breaks=80, xlim=fr, main=mt, xlab="Distance Traveled (Nmi)",
       ylab="Number of Tag Returns")
}

empdisp3<-function()
{
  nf<-layout(matrix(c(1,2,3),3,1,byrow=TRUE))
  layout.show(nf)
  empdisplace("SSS.csv", "SSAP Skipjack")
  empdisplace("RTS.csv", "RTTP Skipjack")
  empdisplace("RTY.csv", "RTTP Yellowfin")

  dev.copy(device=postscript, file="empdisp3.ps", horizontal=FALSE, 
     paper="letter", height=9,width=6,pointsize=12)
  dev.off() 

  dev.copy(device=png,file="empdisp3.png")
  dev.off() 

}

comboplot<-function()
{
  releases<-rttpyft
  empfile <-paste("RTY.csv")
  print(empfile)
  title <- paste("RTTP Yellowfin")
  recaps<-read.csv(empfile,header=T)
  fr<-range(0,1000)
  oldpar <- par(no.readonly = TRUE)
  par(mar=c(5, 5, 5, 5) + 0.1)
  hist(recaps$distnm,breaks=80, xlim=fr, main=title, axes=F, xlab=NULL, ylab=NULL)
  axis(1)
  mtext(text = "Distance Traveled (Nmi)", side = 1,line=3)
  axis(2)
  mtext(text="Recaptured Tags",side=2,line=3)

  require(modreg)
  fit<-paste("fit0b_pdispl_q")
  file<-paste(fit,".prn",sep="")
  print(file)
  disp<-dispread(file)
  dd<-disp[,-1]
  surv<-apply(dd,1,sum)
  deaths<- -1.0*diff(c(releases,surv))
  tt<-apply(dd,2,function(x)x*deaths/(surv+1e-8))
  x<-as.numeric(substring(names(dd),2))
  dpr<-apply(tt,2,sum)
  sdpr<-supsmu(x,dpr)

  par(new = TRUE)
  plot(sdpr$x,sdpr$y, type = "n", xlim=fr,ylim<-c(0,3500), axes = FALSE, xlab="", ylab ="")
  lines(sdpr$x,sdpr$y,col="blue",xlab="", ylab ="",lwd=2)
  axis(4)
  mtext(text="Tags At Liberty",side=4, col="blue",line=3)

  cdpr<-cumsum(dpr)/sum(dpr)
  cprmed<-approx(cdpr,x,.5)
  print(paste("Median displacement distance = ",cprmed$y," NMi",sep=""))
# lines(x,cdpr*max(dpr))
# lines(c(cprmed$y,cprmed$y),c(0,0.5*max(dpr)))
# arrows(cprmed$y,0.0,cprmed$y,0.5*max(dpr),code=1,angle=15,length=0.25,col="green")
  box()
  par(oldpar)

  dev.copy(device=postscript, file="yft_combo.ps", horizontal=FALSE,
     paper="letter", height=6,width=6,pointsize=12)
  dev.off()
# wmf<-paste(fit,".wmf",sep="")
# dev.copy(device=win.metafile,filename = wmf, width = 6, height = 6, 
#          pointsize=12)
# dev.off()
# pngname<-paste(fit,".png",sep="")
# dev.copy(device=png,filename=pngname)
# dev.off()
}

