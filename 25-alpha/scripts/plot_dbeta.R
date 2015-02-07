plot.dbeta<-function(a,b)
{
   x <- seq(0,1,0.01)
   px <-dbeta(x,a,b)
   plot(x,px,type='l')
   print(max(px))
}

plot.multi.beta<-function(file = "allskj-p006-tagest.log")
{
   log<-readLines(file)
   line.patt <- "line 991"
   lines <- grep(line.patt,log)
#  print(lines)
   n <- length(lines)
   a<-vector(len=n)
   b<-vector(len=n)
   i = 0
   for (l in lines)
   {
      i = i+1
   #  print(paste(l,i))
   #  print(paste(l,log[l]))
      a[i] <- as.numeric(strsplit(grep( "tag_surv_prob_",log[l],value=TRUE),split="[ ,]")[[1]][9])
      b[i] <- as.numeric(strsplit(grep( "tag_surv_prob_",log[l],value=TRUE),split="[ ,]")[[1]][13])
   #  print(paste(l,a,b))
   }
    print(head(cbind(a,b)),quote=FALSE)
    
    x <- seq(0,1,0.01)
    px <-dbeta(x,a[1],b[1])
    plot(x,px,type='l',ylim=c(0,10),lwd=5)
    for (i in 2:n)
    {
       px <-dbeta(x,a[i],b[i])
       lines(x,px)
       Sys.sleep(0.1)
       lines(x,px,col="white",lwd=3)
    }
}

plot.tag.surv.prob<-function(fit="013")
{
   file = paste("allskj-p",fit,"-tagest.log",sep="")
   print(paste("Reading ",file))
   log<-readLines(file)
   print(length(log))
   prop.patt <- " tag_surv_prop ="
   prop.line <- grep(prop.patt,log,value=TRUE)
   lpl <- length(prop.line)
#  print(lpl)
#  print(prop.line[[lpl]])
#  print(strsplit(prop.line[[lpl]],split="[ ,]"),quote=FALSE)
   print(paste("prop.line: ",prop.line))
   surv.prop <- strsplit(prop.line[[lpl]],split="[ ,]")
#  print(surv.prop)
#  print(surv.prop[[1]][14:41])
   surv.prop<-as.numeric(surv.prop[[1]][10:37])
#  surv.prop<-as.numeric(surv.prop[[1]][14:41])
   print(surv.prop)

   shape.patt <- "tag_surv_prob_a"
   shape.line <- grep(shape.patt,log,value=TRUE)
   lsl <- length(shape.line)
   print(paste("shape.line: ",shape.line[lsl]))
#  print(strsplit(shape.line[[lsl]],split="[ ,]"),quote=FALSE)
   a <- as.numeric(strsplit(shape.line[[lsl]],split="[ ,]")[[1]][9])
   b <- as.numeric(strsplit(shape.line[[lsl]],split="[ ,]")[[1]][13])
   print(paste("a = ",a,"; b = ",b,sep=""))

   beta.mean <- a/(a+b)
   beta.var <- (a*b)/(((a+b)^2)*(a+b+1))
   beta.sd <- sqrt(beta.var)
   oldpar <- par(no.readonly = TRUE)
   par(mar=c(5, 5, 5, 5) + 0.1)#,las=1)

   hist(surv.prop,breaks=seq(0,1,.1),#freq=FALSE,
       xlab="Tagging Survival Proportion",main="") 
                  
   x <- seq(0,1,0.01)
   px <-dbeta(x,a,b)
   par(new=TRUE)
   plot(x,px,type='l',ann=FALSE,axes=FALSE,col="blue")
   axis(side=4,col.ticks="blue")
   mtext(text="Beta p(x)",side=4,col="blue",line=3)
   abline(v=beta.mean,col="blue",lwd=3)
   sdx1 <- beta.mean - beta.sd
   sdx2 <- beta.mean + beta.sd
   sdy <- max(px)/2
   if (is.infinite(sdy))
     dy = max(surv.prop)/2
   lines(c(sdx1,sdx2),c(sdy,sdy),col="blue",lwd=3)
   box(lwd=2)
   par(oldpar)
 
   bprop <- as.numeric(dbeta(surv.prop,a,b))
   return(as.data.frame(cbind(as.numeric(surv.prop),bprop)))
}

#http://en.wikipedia.org/wiki/Beta_distribution 
plot.moments<-function(a=1.1,b=1.1,n=10)
{
   x<-sort(rbeta(n,a,b))
#  print(x)
   sample.mean <- mean(x)
   sample.var <- var(x)
   print(paste("sample mean = ",sample.mean,"; sample variance = ",sample.var,sep=""))

   beta.mean <- a/(a+b)
   beta.var <- (a*b)/(((a+b)^2)*(a+b+1))
   print(paste("beta mean = ",beta.mean,"; beta variance = ",beta.var,sep=""))

   temp = ((sample.mean*(1-sample.mean))/sample.var) - 1
   est.a = sample.mean*temp
   est.b = (1-sample.mean)*temp
   print(paste("input a = ",a,"; input b = ",b,sep=""))
   print(paste("estimated a = ",est.a,"; estimated b = ",est.b,sep=""))
  
   oldpar <- par(no.readonly = TRUE)
   par(mar=c(5, 5, 5, 5) + 0.1)#,las=1)

   hist(x)

   px <-dbeta(x,a,b)
   par(new=TRUE)
   plot(x,px,type='l',ann=FALSE,axes=FALSE,col="blue")
   axis(side=4,col.ticks="blue")
   mtext(text="Beta p(x)",side=4,col="blue",line=3)
   box(lwd=2)
   par(oldpar)
 
}


