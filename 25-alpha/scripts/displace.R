path <- "/home/jsibert/movemod/SPC2011/"
#uv.active == 1
all.fits <- c("Gen/skj1/gen2.p0c",
              "Gen/skj1/gen1.p07",
              "OneDegree/Gen/skj1/gen2.p05")
prognames<-c("ssap","rttp","pttp")

pretty=TRUE
blwd <- 5
bcol<-c("darkblue","purple4","darkgreen")
fcol<-c("lightblue","purple1","lightgreen")


#fit05_pdispl.csv

dispread<-function(fit.path,op="p",dir="")
{
# print(fit.path)
  path.vec<-unlist(strsplit(fit.path,"/"))
  l <-length(path.vec)

  par<-unlist(strsplit(path.vec[l],".",fixed=T))[2]
  fit<-gsub("p","fit",par)
  dispfile<-""
  for (p in 2:(l-1))
    dispfile<-paste(dispfile,path.vec[p],sep="/")
# print(dispfile) 
  dispfile<-paste(dispfile,"/",fit,"_",op,dir,"displ.csv",sep="")
  print(paste("Reading displacments from file",dispfile))
  disp <- read.csv(file=dispfile,header=TRUE,skip=3)
  return(disp)
}

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

dispplot<-function(prog)
{
   k<-which(prognames == prog)
   file <-paste(path,prog,"/",all.fits[k],sep="")
   disp<-dispread(file)
#  print(disp[1,])
   xtitle <- "Net Displacement (Nmi)"
   x<-as.numeric(gsub("X","",gsub("X\\.","-",names(disp)[-1])))
   mtitle<-toupper(prog)
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

#  ps<-paste(fit,".ps",sep="")
#  dev.copy(device=postscript, file=ps, horizontal=FALSE,
#     paper="letter", height=6,width=6,pointsize=12)
#  dev.off()
}


dpr.comp<-function(disp)
{
## remove first column (months at liberty)
  dd<-disp[,-1]
## compute sum of each row survival by month
  surv<-apply(dd,1,sum)
## compute difference between successive elements
## ie losses per month
  deaths<- -1.0*diff(surv)

# surv<-surv[1:(length(surv)-1)]
## multiply dislacement by mortality rate
## omitting first row of dd and first element of surv
  tt<-apply(dd[-1,],2,function(x)x*deaths/(surv[-1]+1e-8))
  print(dim(tt))
  x<-as.numeric(substring(names(dd),2))
# print(length(x))
  dpr<-apply(tt,2,sum)
# print(length(dpr))
  cdpr<-cumsum(dpr)/sum(dpr)
  cprmed<-approx(cdpr,x,.5)
  print(paste("Median displacement distance = ",cprmed$y," NMi",sep=""))

  return(list(dpr=dpr,cdpr=cdpr,cprmed=cprmed))
}

udisplot<-function(prog,spp)
{
   k<-which(prognames == prog)
   file <-paste(path,prog,"/",all.fits[k],sep="")
   disp<-dispread(file)
   dpr.list<-dpr.comp(disp)
   mtitle<-paste(toupper(prog),spp)
   x<-as.numeric(substring(names(disp[,-1]),2))
   dpr<-dpr.list$dpr
#  print(range(dpr))
   plot(x,dpr,type='l',xlim<-c(1,5000),lwd=2, main=mtitle,
        xlab="Net Displacement (NMi)",ylab="Number of Fish")
   rug(x,ticksize=0.015,quiet=TRUE)

#  pts<-2
#  fpts<-pts/length(x)
#  sdpr<-supsmu(x,dpr.list$dpr,span=fpts)
#  lines(sdpr$x,sdpr$y,lwd=pts)

#  pts<-6
#  fpts<-pts/length(x)
#  sdpr<-supsmu(x,dpr.list$dpr,span=fpts)
#  lines(sdpr$x,sdpr$y,lwd=pts)

#  sdpr<-dpr.list$sdpr #supsmu(x,dpr)
#  lines(sdpr$x,sdpr$y,lty="dotdash")
   cdpr<-dpr.list$cdpr #cumsum(dpr)/sum(dpr)
   cprmed<-dpr.list$cprmed #approx(cdpr,x,.5)
#  print(paste("Median displacement distance = ",cprmed$y," NMi",sep=""))
   lines(x,cdpr*max(dpr),col="blue")
   arrows(cprmed$y,0.0,cprmed$y,0.5*max(dpr),code=1,angle=15,length=0.25,col="green")

#  ps<-paste(fit,".ps",sep="")
#  dev.copy(device=postscript, file=ps, horizontal=FALSE,
#     paper="letter", height=6,width=6,pointsize=12)
#  dev.off()
#  return(dpr.list)
}

udisplot3<-function(spp="Skipjack")
{
#  k<-which(prognames == "ssap")
#  file <-paste(path,"ssap","/",all.fits[k],sep="")
#  ssap.dpr<-dispread(file)

#  k<-which(prognames == "rttp")
#  file <-paste(path,prog,"/",all.fits[k],sep="")
#  rttp.dpr<-dispread(file)

#  k<-which(prognames == "pttp")
#  file <-paste(path,prog,"/",all.fits[k],sep="")
#  pttp.dpr<-dispread(file)

#  x<-as.numeric(substring(names(ssap.disp[,-1]),2))
#  yrange<-c(0.0,max(ssap.
#  return(ssap.dpr)
 
   ndpr <- 1868
   dpr.mat<-matrix(nrow=ndpr,ncol=3)
   cumdpr.mat<-matrix(nrow=ndpr,ncol=3)
   cprmed<-vector(length=3)
   print(dim(dpr.mat))

   for (prog in prognames)
   {
       k <- which(prognames==prog)
       print(paste(prog,k))
       file <-paste(path,prog,"/",all.fits[k],sep="")
       disp <- dispread(file)
       dpr.list<-dpr.comp(disp)
       dpr<-dpr.list$dpr
       x<-as.numeric(substring(names(disp[,-1]),2))
       print(paste(length(dpr),length(x)))

#      x11()
#      plot(x,dpr,type='l')
#      print(paste(length(dpr),length(x)))
#      print(dim(dpr.mat))

       dpr.mat[,k]<-dpr

       cumdpr.mat[,k]<-dpr.list$cdpr #cumsum(dpr)/sum(dpr)
       cprmed[k]<-dpr.list$cprmed$y #approx(cdpr,x,.5)
       print(paste("yMedian displacement distance = ",cprmed[k]," NMi",sep=""))
   }

   dmax <-2500
   w <- which(x<=dmax)
   ww <- w[length(w)]
   xrange<-c(0,1.08*dmax) #range(x)
   yrange<-range(dpr.mat,na.rm=TRUE)
   def.par <- par(no.readonly = TRUE) 
   par(mar=(c(4,5,1,3)+0.1))
 
   plot(xrange,yrange,type='n',axes=FALSE, xlab="Net Displacement (NMi)",ylab="Number of Tags")
   axis(1,lwd=0)
   axis(2,las=1,lwd=0)
   abline(h=par("usr")[3],lwd=3)
   abline(v=par("usr")[1],lwd=3)


   for (k in 1:3)
   {
      y <- dpr.mat[,k]
      points(x[w],y[w],col=bcol[k],pch=16)
      y<-cumdpr.mat[,k]*max(dpr.mat[,k])
      arrows(cprmed[k],0.0,cprmed[k],0.5*max(dpr.mat[,k]),code=1,angle=15,
             length=0.25,col=bcol[k])
      double.lines(x[w],y[w],bcol=bcol[k],fcol=fcol[k],lwd=blwd)

      text(dmax, y[ww],adj=c(0.0,0.5), cex=1.2,
           labels=paste(" ",toupper(prognames[k]),sep=""), col = bcol[k])
      
   }
   par(def.par)
   return(dpr.mat)
}


udisplot2<-function(fit1,fit2,releases)
{
# require(modreg)
  file<-paste(fit1,".prn",sep="")
  disp<-dispread(file)

  dd<-disp[,-1]
  surv<-apply(dd,1,sum)
  deaths<- -1.0*diff(c(releases,surv))
  tt<-apply(dd,2,function(x)x*deaths/(surv+1e-8))
  x<-as.numeric(substring(names(dd),2))
  dpr<-apply(tt,2,sum)
  sdpr<-supsmu(x,dpr)
  cdpr<-cumsum(dpr)/sum(dpr)
  cprmed<-approx(cdpr,x,.5)

  print(paste("Median displacement distance = ",cprmed$y," NMi",sep=""))
  lsty<-"solid"
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

# require(modreg)
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

make.displacement.bins<-function(ngrid=100)
{
  gins <- sort(unique(60*as.vector(outer(0:ngrid,0:ngrid,function(x,y)sqrt(x^2+y^2)))))
  print(length(gins))
# print(tail(gins))

# w1 <- which(gins <= 1000)
# print(length(w1))
# print(tail(gins[w1]))

# w2 <- which(diff(gins) > 1)
# print(length(w2))
# print(head(gins[w2]))
# print(tail(gins[w2]))

# ww <- unique(c(w1,w2))
# print(length(ww))
# print(head(gins[ww]))
# print(tail(gins[ww]))

  write(gins,file=paste("gins",ngrid,".csv",sep=""),ncol=6,sep=",")
  return(gins)
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

