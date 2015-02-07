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

like.prof.plot<-function(file)
{
   plt.file = paste(file,".plt",sep="")
   lpf <- read.profile(plt.file)
   attach(lpf)
   lwd = 3
   plot(prof.like$x,prof.like$Px,type='b',lwd=lwd+2,col="blue",
        xlab="x",ylab="P(x)")
   lines(norm.approx$x,norm.approx$Px,lwd=lwd-2,col="red")

   detach(lpf)
   save.png.plot(file)
}


save.png.plot<-function(gfile,width=9,height=6.5)
{
     gfile.eps <- paste(gfile,".eps",sep="")
     gfile.png <- paste(gfile,".png",sep="")
     dev.copy2eps(file=gfile.eps,width=width,height=height)
     print(paste("saved",gfile.eps))
#    ps2raster is part of GMT
     cmd <- paste("ps2raster -V -A -Tg -E300",gfile.eps)
#    alternatively use convert from Imagemagick
#    cmd <- paste("convert -antialias -density 300",file.pdf,file.png,sep=" ")
#    cmd <- paste("pstoimg -antialias -aaliastext -quiet -density=",300,
#                  " -type png -crop a -out ",gfile.png," ", gfile.eps,sep="")
     print(paste("Executing",cmd))
     print("Converting eps to png; this may take a while.")
     system(cmd)
     print(paste("saved",gfile.png))
     system(paste("rm -fv ",gfile.eps,sep=""))
}


hist.plot<-function(root,fit,legend.text=NULL)
{
   file<-paste(root,"-p",fit,"-hist.csv",sep="")
   print(file)
   dat<-read.csv(file,skip=0,hea=TRUE)
   print(dim(dat))
   print(head(dat))
   mal <- dat$t

   
   gfile<-paste(root,"-p",fit,"-history",sep="")


   if (!interactive())
   {
      pdf(file=paste(gfile,".pdf",sep=""),width=9,height=6.5)
   }

   lwd <- 3
   log <- ""
   ndate <- nrow(dat)
   edate<-seq(from=as.Date(paste(dat$year[1],dat$month[1],1,sep='-')),
               to=as.Date(paste(dat$year[ndate],dat$month[ndate],1,sep='-')),by="1 month")
   print(range(edate))
   yrange<-c(0,max(dat$obs,dat$pred))
   if (ncol(dat) == 7)
     yrange<-c(0,max(dat$obsT,dat$pred))
   print(yrange)

 # oldpar <- par(no.readonly = TRUE)
 # par(mar=c(5, 4, 4, 4) + 0.1)#,las=1)

   #obs <- ota$with.effort[ndx]
   #pred <- pta$All[ndx]
   lwd <- 3
   col <- 4
   plot(edate,dat$obs, ylim=yrange, pch=3, xlab="Date",ylab="Recaptures",cex=1.6, col=col)
   lines(edate,dat$pred,lwd=lwd,col=col)

   if (ncol(dat) >= 7)
   {
      points(edate,dat$obs0,pch=1,col=col)
      points(edate,dat$obsT,pch=5,col=col)
   }

   if (is.null(legend.text))
   {
     legend.text = c(fit,"With Effort", "Without Effort","Total")
     legend.pch = c(NA,3,1,5)
     legend.lty = c(1,0,0,0)
   }
   legend("topright",bty='n',legend=legend.text,pch=legend.pch,
           lty=legend.lty, col=col)

   if (ncol(dat) == 8)
   {
      rdev<-exp(dat$rdev)
      for (i in 1:length(edate))
      {
         x<-c(edate[i],edate[i])
      #  print(x)
      #  y<-c(dat$pred[i],rdev[i]/dat$pred[i])
      #  print(y)
      #  lines(x,y)
     
         lines(c(edate[i],edate[i]),c(dat$pred[i],rdev[i]/dat$pred[i]),col=col,lty="dotted")

      }
      
   #  par(new=TRUE)
   #  rdev<-exp(dat$rdev)
   #  plot(edate,rdev,type='l',lty="dotted",ann=FALSE,axes=FALSE,col=col)
   #  axis(side=4)#,col=col,col.ticks=col)
   #  mtext(text="Recapture Deviations",side=4,col=col,line=3) #,outer=FALSE)
   }
   if (interactive())
   {
     save.png.plot(gfile)
   }
#  par(oldpar)
  
}

triple.ta.plot<-function(root,fit,legend.text=NULL)
{
   width <- 6.5
   height <- 9.0
   x11(width-width,height=height)
   op <- par(no.readonly = TRUE)
   par(mar=c(0,4,0.1,0.1)+0.0) #,mgp=c(3,0,0)) #,las=1)
   nf <- layout(matrix(c(1,2,3,4),ncol=1,nrow=,4,byrow=TRUE))
   print(nf)
   layout.show(nf)
   ta.plot(root,fit,'n',save.graphics=FALSE)
   ta.plot(root,fit,'r',save.graphics=FALSE)
   ta.plot(root,fit,'y',save.graphics=FALSE)
   mtext("Months at Liberty",side=1,line=-2,cex=0.8)
   plot(c(0,1),c(0,1),ann=FALSE,axes=FALSE,type='n')
   mtext("Months at Liberty",side=3,line=-3,cex=0.8,col="red")

   gfile<-paste(root,"-p",fit,"-attrition",sep="")
   save.png.plot(gfile,width,height)

   par(op)
}


ta.plot<-function(root,fit,transform='r',legend.text=NULL,save.graphics=TRUE)
{
   file<-paste(root,"-p",fit,"-attr.csv",sep="")
   print(file)
   dat<-read.csv(file,skip=0,hea=TRUE)
   print(head(dat))
   mal <- dat$t
   obs <- dat$obs

#  print(pta.list)
#  pta<-lapply(pta.list,.pta.read)


   gfile<-paste(root,"-p",fit,"-attrition-",transform,sep="")
   if (!interactive())
   {
      pdf(file=paste(gfile,".pdf",sep=""),width=9,height=6.5)
   }

   lwd <- 3
   col <- 4
   log <- ""
   xrange <- c(0,nrow(dat))
   arange<-c(0,max(dat$obs,dat$pred))
   if (ncol(dat) == 5)
      arange<-c(0,max(dat$obsT,dat$pred))
      
   obs.pch <- 3
   
   if (transform != 'r')
   {
     if (transform == 'y')
     {
       log = 'y'
       arange[1] = 0.9
     }
    plot(mal,obs,xlim=xrange,ylim=arange,pch=obs.pch,xlab="Months at Liberty",ylab="Tag Returns",cex=1.6,log=log)
   # plot(mal,obs,xlim=xrange,ylim=arange,pch=obs.pch,cex=1.6,log=log,ann=FALSE)
     points(mal,dat$obs0,pch=1,col=col)
     points(mal,dat$obsT,pch=5,col=col)
   # mtext("Months at Liberty",side=1,line=2)
   # mtext("Tag Returns",side=2,line=3)
   }
   else
   {
     obs <- sqrt(obs)
     plot(xrange,sqrt(arange),type='n',axes=FALSE,xlab="Months at Liberty",ylab="Tag Returns")
     points(mal,obs,pch=obs.pch,cex=1.6)
     if (ncol(dat) == 5)
     {
        points(mal,sqrt(dat$obs0),pch=1,col=col)
        points(mal,sqrt(dat$obsT),pch=5,col=col)
     }
   }

#  for (f in 1:length(pta))
#  {
#    if (transform != 'r')
#      lines(pta[[f]]$liberty,pta[[f]]$All,col=f+3,lwd=lwd)
#    else
#      lines(pta[[f]]$liberty,sqrt(pta[[f]]$All),col=f+3,lwd=lwd)
#  }
   if (transform != 'r')
      lines(dat$t,dat$pred,col=col,lwd=lwd)
   else
      lines(dat$t,sqrt(dat$pred),col=col,lwd=lwd)

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
   {
     legend.text = c(fit,"With Effort", "Without Effort","Total")
     legend.pch = c(NA,3,1,5)
     legend.lty = c(1,0,0,0)
   }
   legend("topright",bty='n',legend=legend.text,pch=legend.pch,
           lty=legend.lty, col=col)

   if (interactive() && save.graphics)
   {
     save.png.plot(gfile)
   }

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
