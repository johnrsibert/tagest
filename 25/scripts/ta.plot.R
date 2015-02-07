`ta.plot` <-
function(ota.file,pta.list,transform='n',nmonth=36,legend.text=NULL)
{
   ota<-read.csv(ota.file,skip=2,hea=T)
   ndx<-which(ota$All>1e-8)

   mal <- ota$liberty[ndx]
   obs <- ota$All[ndx]

   pta<-lapply(pta.list,.pta.read)

   arange<-range(obs,lapply(pta,.ta.range))

   op <- par(no.readonly = TRUE)

   lwd <- 3
   log <- ""
   xrange <- c(0,nmonth)
   
   if (transform != 'r')
   {
     if (transform == 'y')
       log = 'y'
     plot(mal,obs,xlim=xrange,ylim=arange,pch='+',xlab="Months at Liberty",ylab="Tag Returns",cex=1.2,log=log)
   }
   else
   {
     obs <- sqrt(obs)
     plot(xrange,sqrt(arange),type='n',axes=FALSE,xlab="Months at Liberty",ylab="Tag Returns")
     points(mal,obs,pch='+',cex=1.2)
   }

   for (f in 1:length(pta))
   {
     if (transform != 'r')
       lines(pta[[f]]$liberty,pta[[f]]$All,col=f+1,lwd=lwd)
     else
       lines(pta[[f]]$liberty,sqrt(pta[[f]]$All),col=f+1,lwd=lwd)

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
   legend("topright",bty='n',legend=legend.text,col=seq(2,(length(pta)+1)),lwd=lwd)


   par(op)

   gfile<-paste("attrition",transform,sep="_")
   gfile.eps <- paste(gfile,".eps",sep="")
   gfile.png <- paste(gfile,".png",sep="")
   dev.copy2eps(file=gfile.eps,width=9,height=6.5)
   print(paste("saved",gfile.eps))
   cmd<-paste("pstoimg -antialias -aaliastext -quiet -density=",300,
                 " -type png -crop a -out ",gfile.png," ", gfile.eps,sep="")
   print(paste("Executing",cmd))
   print("Converting eps to png; this may take a while.")
   system(cmd)
   print(paste("saved",gfile.png))
}

