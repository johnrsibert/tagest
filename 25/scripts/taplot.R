.pta.read<-function(file)
{
   print(paste("in .pta.read",file))
   pta<-read.csv(file,skip=2,hea=T)
   ndx<-which(pta$All > 0)
   tmp<-cbind(pta$liberty[ndx],pta$All[ndx])
   colnames(tmp)<-c("liberty","All")
   return(as.data.frame(tmp))
}

.ta.range<-function(ta)
{
  return(range(ta$All))
}

.ta.max<-function(ta)
{
  return(max(ta$All))
}

read.profile<-function(plt.file)
{
   sca<-scan(file=plt.file,what="raw")
   Profile.pos <- grep("Profile",sca)
#  print(Profile.pos)
   Minimum.pos <- grep("Minimum",sca)
#  print(Minimum.pos)
   Normal.pos <- grep("Normal",sca)
#  print(Normal.pos)

   lp.start <- Profile.pos + 2;
   lp.end <- Minimum.pos[1] - 2;
   lp.ndx <- seq(lp.start,lp.end,2)
#  print(sca[lp.ndx])
#  print(sca[(lp.ndx)+1])

   na.start <- Normal.pos[1] + 2
   na.end <- Minimum.pos[2] - 2;
   na.ndx <- seq(na.start,na.end,2)
#  print(sca[na.ndx])
#  print(sca[(na.ndx)+1])

   prof.like <- cbind(as.numeric(sca[lp.ndx]), as.numeric(sca[(lp.ndx)+1]))
   norm.approx <- cbind(as.numeric(sca[na.ndx]), as.numeric(sca[(na.ndx)+1]))
   colnames(prof.like) <-c("x","Px")
   colnames(norm.approx) <-c("x","Px)")

   return(list(prof.like=as.data.frame(prof.like),norm.approx=as.data.frame(norm.approx)))

}

like.prof.plot<-function(plt.file)
{
   lpf <- read.profile(plt.file)
   attach(lpf)
   lwd = 3
   plot(prof.like$x,prof.like$Px,type='b',lwd=lwd+2,col="blue",
        xlab="x",ylab="P(x)")
   lines(norm.approx$x,norm.approx$Px,lwd=lwd-2,col="red")

   detach(lpf)
}

#hist.plot<-function(hex.fit)
hist.plot<-function(obs.fit,pta.list,legend.text=NULL)
{
   ota.file<-paste("fit",obs.fit,"_P1_ota.csv",sep="")
   print(ota.file)
   ota<-read.csv(ota.file,skip=1,hea=T)
   print(head(ota))
   ndx<-which(ota$with.effort>1)
   mal <- ota$month[ndx]

#  pta.file<-paste("fit",hex.fit,"_P1_pta.csv",sep="")
#  print(pta.file)
#  pta<-read.csv(pta.file,skip=2,hea=T)
#  print(head(pta))
   
   print(pta.list)
  
   pta.files <- vector(length=length(pta.list))
   for (f in 1:length(pta.list))
   {
       pta.files[f] <-  paste("fit",pta.list[f],"_P1_pta.csv",sep="")
   }
   print(pta.files)
   pta<-lapply(pta.files,.pta.read)


#  gfile<-paste("fit",hex.fit,"_history",sep="")
   gfile<-"history"


   if (!interactive())
   {
      pdf(file=paste(gfile,".pdf",sep=""),width=9,height=6.5)
   }

   lwd <- 3
   log <- ""
   ndate <- nrow(ota)
   edate<-seq(from=as.Date(paste(ota$YYYY[1],ota$MM[1],1,sep='-')),
               to=as.Date(paste(ota$YYYY[ndate],ota$MM[ndate],1,sep='-')),by="1 month")
   yrange<-c(0,max(ota$total))
   print(yrange)


   #obs <- ota$with.effort[ndx]
   #pred <- pta$All[ndx]
   lwd <- 3
   col <- 4
   plot(edate[ndx],ota$with.effort[ndx], ylim=yrange, pch=3, xlab="Date",ylab="Recaptures",cex=1.6, col=col)
   points(edate[ndx],ota$effortless[ndx], pch=1, cex=0.8, col="red")
   points(edate[ndx],ota$total[ndx], pch=1, cex=0.8, col="purple")
   rug(edate[ndx],ticksize=-0.015)
   
   for (f in 1:length(pta))
   {
     lines(edate[ndx],pta[[f]]$All[ndx],lwd=lwd,col=f+3)
   }

   if (is.null(legend.text))
     legend.text = pta.list
   legend("topright",bty='n',legend=legend.text,col=seq(4,(length(pta)+4)),lwd=lwd)
   legend("topleft",bty='n',legend=c("With effort","Without effort","Total"),
                             col=c(col,"red","purple"),
                             pch=c(3,1,1),pt.cex=c(1.6,0.8,0.8))

   if (interactive())
   {
     gfile.eps <- paste(gfile,".eps",sep="")
     gfile.png <- paste(gfile,".png",sep="")
     dev.copy2eps(file=gfile.eps,width=9,height=6.5)
     print(paste("saved",gfile.eps))
     cmd <- paste("ps2raster -V -A -Tg -E300",gfile.eps)
   # cmd<-paste("pstoimg -antialias -aaliastext -quiet -density=",300,
   #               " -type png -crop a -out ",gfile.png," ", gfile.eps,sep="")
     print(paste("Executing",cmd))
     print("Converting eps to png; this may take a while.")
     system(cmd)
     print(paste("saved",gfile.png))
   }
# cmd <- paste("convert -antialias -density 300",file.pdf,file.png,sep=" ")
 
}

ta.plot<-function(ota.file,pta.list,transform="r",nmonth=36,legend.text=NULL)
{
   print(ota.file)
   ota<-read.csv(ota.file,skip=1,hea=T)
   ndx<-which(ota$with.effort>1e-8)

   mal <- ota$month[ndx]
   obs <- ota$with.effort[ndx]

   print(pta.list)
   pta<-lapply(pta.list,.pta.read)


   gfile<-paste("attrition",transform,sep="_")
   op <- par(no.readonly = TRUE)
   if (!interactive())
   {
      pdf(file=paste(gfile,".pdf",sep=""),width=9,height=6.5)
   }

   lwd <- 3
   log <- ""
   xrange <- c(0,nmonth)
   arange<-range(obs,lapply(pta,.ta.range))
   obs.pch <- 20
   
   if (transform != 'r')
   {
     if (transform == 'y')
     {
       arange <- range(c(0.9,lapply(pta,.ta.max)))
       log = 'y'
     }
     plot(mal,obs,xlim=xrange,ylim=arange,pch=obs.pch,xlab="Months at Liberty",ylab="Tag Returns",cex=1.2,log=log)
   }
   else
   {
     obs <- sqrt(obs)
     plot(xrange,sqrt(arange),type='n',axes=FALSE,xlab="Months at Liberty",ylab="Tag Returns")
     points(mal,obs,pch=obs.pch,cex=1.2)
   }

   for (f in 1:length(pta))
   {
     if (transform != 'r')
       lines(pta[[f]]$liberty,pta[[f]]$All,col=f+3,lwd=lwd)
     else
       lines(pta[[f]]$liberty,sqrt(pta[[f]]$All),col=f+3,lwd=lwd)

   }
   if (transform == 'r')
   {
     at <- c(0,4)
     k <- max(at)
     while (k < arange[2])
     {
       k <- k*2
       at<-c(at,k)
     }
     axis(1)
     axis(2,at=sqrt(at),labels=as.character(at))
     box()

   }
   if (is.null(legend.text))
     legend.text = pta.list
   legend("topright",bty='n',legend=legend.text,col=seq(4,(length(pta)+4)),lwd=lwd)


   par(op)

   if (interactive())
   {
     gfile.eps <- paste(gfile,".eps",sep="")
     gfile.png <- paste(gfile,".png",sep="")
     dev.copy2eps(file=gfile.eps,width=9,height=6.5)
     print(paste("saved",gfile.eps))
     cmd <- paste("ps2raster -V -A -Tg -E300",gfile.eps)
  #  cmd<-paste("pstoimg -antialias -aaliastext -quiet -density=",300,
  #                " -type png -crop a -out ",gfile.png," ", gfile.eps,sep="")
     print(paste("Executing",cmd))
     print("Converting eps to png; this may take a while.")
     system(cmd)
     print(paste("saved",gfile.png))
   }
# cmd <- paste("convert -antialias -density 300",file.pdf,file.png,sep=" ")
 
}

# these are preliminary versions and probably shouldn't be used
# the leading . makes them 'invisible'
.ta.plot.1<-function(ota.file,pta.file)
{
   ota<-read.csv(ota.file,skip=2,hea=T)
   print(dim(ota))
   pta<-read.csv(pta.file,skip=2,hea=T)
   print(dim(pta))

   arange<-range(ota$All,pta$All)
   print(arange)

   plot(ota$liberty,ota$All,ylim=arange,pch='+')
   lines(pta$liberty,pta$All)
}

.ta.plot.2<-function(ota.file,pta.list)
{
   ota<-read.csv(ota.file,skip=2,hea=T)

   pta<-lapply(pta.list,pta.read)

   arange<-range(ota$All,pta)

   lwd <- 3
   plot(ota$liberty,ota$All,ylim=arange,pch='+',xlab="Months at Liberty",ylab="Rag Returns",cex=1.2)
   for (f in 1:length(pta))
   {
     lines(ota$liberty[1:length(pta[[f]])],pta[[f]],col=f+1,lwd=lwd)
   }
   print(seq(2,(length(pta)+1)))
   legend("topright",legend=pta.list,col=seq(2,(length(pta)+1)),lwd=lwd)


   gfile<-"attrition"
   gfile.eps <- paste(gfile,".eps",sep="")
   gfile.png <- paste(gfile,".png",sep="")
   dev.copy2eps(file=gfile.eps,width=9,height=6.5)
   print(paste("saved",gfile.eps))
   cmd<-paste("pstoimg -antialias -aaliastext -quiet -density=",300,
                 " -type png -crop a -out ",gfile.png," ", gfile.eps,sep="")
   print(paste("executing",cmd))
   system(cmd)
   print(paste("saved",gfile.png))
 
}

hist.plot.rough<-function(ota.file,pta.file,transform='n',nmonth=NULL,legend.text=NULL)
{
   print(ota.file)
   print(pta.file)
   ota<-read.csv(ota.file,skip=2,hea=T)
   pta<-read.csv(pta.file,skip=2,hea=T)
   ndx<-which(ota$All>1e-8)

   mal <- ota$liberty[ndx]

   if (is.null(nmonth))
   {
     nmonth = ota$liberty[nrow(ota)]
   }
   print(nmonth)


   gfile<-paste("history",transform,sep="_")
   op <- par(no.readonly = TRUE)
   if (!interactive())
   {
      pdf(file=paste(gfile,".pdf",sep=""),width=9,height=6.5)
   }

   lwd <- 3
   log <- ""
   xrange <- range(mal) #c(0,nmonth)
   arange<-c(0,max(ota$All,pta$All))
   print(arange)

   fleet.cols<-seq(3,ncol(ota))   
   print(fleet.cols)

   if (transform == 'y')
   {
      arange <- c(0.9,max(ota$All,pta$All))
      log = 'y'
   }
   else if (transform == 'r')
   {
     arange <- sqrt(arange)
   }
   print(xrange)
   print(arange)

   plot(xrange,arange,type='n',log=log,axes=FALSE,
         xlab="Months from First Release",ylab="Tag Returns")

   palette(rev(rainbow(ncol(ota))))
   fleet.names <- colnames(ota)
   fleet.lwd <- vector(length=length(fleet.names))
   fleet.cex <- vector(length=length(fleet.names))

   for (ff in fleet.cols)
   {
     print(paste(ff,fleet.names[ff]))
     obs <- ota[ndx,ff]
     pred <- pta[ndx,ff]
     if (transform == 'r')
     {
        obs = sqrt(obs)
        pred = sqrt(pred)
     }
     if ( (!is.na(pmatch("id",fleet.names[ff]))) ||
          (!is.na(pmatch("ph",fleet.names[ff]))) ||
          (!is.na(pmatch("All",fleet.names[ff]))) )
     {
       fleet.lwd[ff] <- 3
       fleet.cex[ff] <- 2
     }
     else
     {
       fleet.lwd[ff] <- 1
       fleet.cex[ff] <- 1
     }
     points(mal,obs,log=log,col=ff,pch='+',cex=fleet.cex[ff])
     lines(mal,pred,log=log,lty="solid",col=ff,lwd=fleet.lwd[ff])
   }

   if (transform == 'r')
   {
     at <- c(0,4)
     k <- max(at)
     while (k < arange[2]*arange[2])
     {
       k <- k*2
       at<-c(at,k)
     }
     axis(2,at=sqrt(at),labels=as.character(at))

   }
   else
   {
      axis(2)
   }
   axis(1)
   box()

   print(fleet.lwd)
   if (is.null(legend.text))
     legend.text = colnames(ota)[3:ncol(ota)]
   legend("topright",bty='n',legend=legend.text,col=c(3:ncol(ota)),pch='+',
      lwd=fleet.lwd[3:ncol(ota)],pt.cex=fleet.cex[3:ncol(ota)])


   par(op)

   if (interactive())
   {
     gfile.eps <- paste(gfile,".eps",sep="")
     gfile.png <- paste(gfile,".png",sep="")
     dev.copy2eps(file=gfile.eps,width=9,height=6.5)
     print(paste("saved",gfile.eps))
     cmd <- paste("ps2raster -V -A -Tg -E300",gfile.eps)
     cmd<-paste("pstoimg -antialias -aaliastext -quiet -density=",300,
                   " -type png -crop a -out ",gfile.png," ", gfile.eps,sep="")
     print(paste("Executing",cmd))
     print("Converting eps to png; this may take a while.")
     system(cmd)
     print(paste("saved",gfile.png))
   }
# cmd <- paste("convert -antialias -density 300",file.pdf,file.png,sep=" ")
 
}
