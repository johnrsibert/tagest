require("movemodutils")
pretty=TRUE
path <- "/home/jsibert/movemod/SPC2011/"
sd.mult <- 2

mort.names<-c("M","F","Z",
              "fmps", "idps", "jpps", "krps", "mhps",
              "pgps", "phps", "sbps", "twps", "usps",
              "vups", "phdo", "iddo", "idpl", "fjpl",
              "jppl", "kipl", "phpl", "pgpl", "sbpl")

all.fits<-matrix(nrow=2,ncol=3)
#uv.active == 0
all.fits[1,] <- c("ssap/Gen/skj1/gen2.p0b",
              "rttp/Gen/skj1/gen1.p08",
              "pttp/OneDegree/Gen/skj1/gen2.p06")
#uv.active == 1
all.fits[2,] <- c("ssap/Gen/skj1/gen2.p0c",
              "rttp/Gen/skj1/gen1.p07",
              "pttp/OneDegree/Gen/skj1/gen2.p05")
D.tab<-function(uv.active=1)
{
  index.tab <- read.csv(paste(path,"GenRegion/RegionNumbering.csv",sep=""),header=TRUE)
  nregion<-nrow(index.tab)
  fits<-all.fits[(uv.active+1),]

  nfit <- length(fits)
  prog <- vector(length=nfit)
  par  <- vector(length=nfit)
  est <- matrix(nrow=nregion,ncol=nfit)
  std <- matrix(nrow=nregion,ncol=nfit)
  print(paste("Processing",nfit,"fits:"))
  for (ff in 1:nfit)
  {
     fit <- fits[ff]
     fitsplit <- unlist(strsplit(fit,"/"))
     prog[ff]<-fitsplit[1]
     par[ff] <- fitsplit[length(fitsplit)]
 
     #par[ff]<-sub(".","-",par[ff],fixed=TRUE)
     print(paste(ff,prog[ff],par[ff]))

     file<-paste(path,sub(".","-",fit,fixed=TRUE),sep="")
     print(paste(ff,file))
     tmp <- read.admb.cor(file)
     w<-which(tmp$names=="ddr")
     nest <- length(w)
     ndx <- w[1] - 1
     col <- ff
     for (rr in 1:nest)
     {
        ndx <- ndx + 1
        row <- index.tab[rr,(ff+2)]
     #  print(paste(rr,ndx,row,col))
        est[row,ff] <- tmp$est[ndx]
        std[row,ff] <- tmp$std[ndx]
     }
  }
  colnames(est)<-prog
  rownames(est)<-index.tab[,1]

  save.as.tex.table(paste("diffusivity",uv.active,".tex",sep=""),rownames(est),colnames(est),
                    est,"Region",fmt="%.1f")
  plot.diffusivity(est,std,uv.active)
  return(list(est=est,std=std))
}

plot.diffusivity<-function(est,std,uv.active)
{
   nregion<-length(rownames(est))
   yrange<-c(0,max(est+std,na.rm=T))
   xrange<-c(1,nregion)
   x <- c(1:nregion)

   width <- 11.0
   height <- 6.0
   x11(width=width,height=height)

   def.par <- par(no.readonly = TRUE) 

   plot(xrange,yrange,type='n',axes=FALSE, xlab="Region",ylab="Diffusion Radius (Nmi/Day)")

   nprog <- ncol(est)
   dx <-0.075
   diff <- vector(length=nregion)
   dfile <- "D_diff_values.dat"
#  cat ("#",file=dfile,sep="\n",append=FALSE)
   cat ("",file=dfile,sep="",append=FALSE)
   for (r in 1:nregion)
   {
     if (is.na(est[r,1]))
        D1 <- est[r,2]
     else
        D1 <- est[r,1]

     if (is.na(est[r,3]))
        D2 <- D1
     else
        D2 <- est[r,3]
      
     diff[r] <- D2 - D1
     cat(paste(sprintf("%3d",r),sprintf("%.3f",diff[r])),file="",sep="\n",append=TRUE)
     cat(paste(sprintf("%3d",r),sprintf("%.3f",diff[r])),file=dfile,sep="\n",append=TRUE)
   }
   print(range(diff))
   print(diff)
   sdiff <-sort(diff,index.return=TRUE,decreasing=FALSE)
   print(sdiff)
#  ix <- sdiff$ix
   ix <- c(1:nregion)

   for (prog in 1:nprog)
   {
     if (prog == 1)
       col <- "blue"
     else if (prog == 2)
       col = "lightblue"
     else if (prog >= 3)
       col = gray((prog-1)/nprog) #prog + 11

     abline(h=median(est[,prog],na.rm=TRUE),col=col,lwd=2,lty="dotdash")

      pch <- toupper(substr(colnames(est)[prog],1,1))
      for (rr in 1:nregion)
      {
        qq <- ix[rr]
        xx <- qq+(prog-2)*dx
        if (!is.na(est[qq,prog]))
        {
        #  double.lines(c(xx,xx), c((est[qq,prog]-sd.mult*std[qq,prog]),(est[qq,prog]+sd.mult*std[qq,prog])), fcol=col,lwd=11)
        #  points(xx, est[qq,prog], col="white", pch=20,cex=1)
           plot.sd.range(xx,est[qq,prog],std[qq,prog],fcol=col)
        }
      }
   }
   axis(1,at=x,labels=rownames(est)[ix],cex.axis=0.8,lwd=0)
   axis(2,las=1,lwd=0)

   #box()
   abline(h=par("usr")[3],lwd=3)
   abline(v=par("usr")[1],lwd=3)

   ltext<-c("1977 - 1982       ","1989 - 1995       ","2006 - 2010")
   lcol<-c("blue","lightblue",gray(2/nprog))
   legend(x=nregion/2,y=yrange[2],legend=ltext,col=lcol,pch=20,pt.cex=3,
          xjust=0.0,yjust=0.5,horiz=TRUE,box.lty=0)


   save.png.plot(paste("diffusivity-change",uv.active,sep=""),width,height)
   par(def.par)
}

uv.tab<-function(direction)
{
  index.tab <- read.csv(paste(path,"GenRegion/RegionNumbering.csv",sep=""),header=TRUE)
  nregion<-nrow(index.tab)
  fits<-all.fits[2,]

  nfit <- length(fits)
  prog <- vector(length=nfit)
  par  <- vector(length=nfit)
  est <- matrix(nrow=nregion,ncol=nfit)
  std <- matrix(nrow=nregion,ncol=nfit)
  print(paste("Processing",nfit,"fits:"))
  for (ff in 1:nfit)
  {
     fit <- fits[ff]
     fitsplit <- unlist(strsplit(fit,"/"))
     prog[ff]<-fitsplit[1]
     par[ff] <- fitsplit[length(fitsplit)]
 
     print(paste(ff,prog[ff],par[ff]))

     file<-paste(path,sub(".","-",fit,fixed=TRUE),sep="")
     print(paste(ff,file))
     tmp <- read.admb.cor(file)
     w<-which(tmp$names==paste("seasonGrid",toupper(direction),sep=""))
     nest <- length(w)
     ndx <- w[1] - 1
     col <- ff
     for (rr in 1:nest)
     {
        ndx <- ndx + 1
        row <- index.tab[rr,(ff+2)]
     #  print(paste(rr,ndx,row,col))
        est[row,ff] <- tmp$est[ndx]
        std[row,ff] <- tmp$std[ndx]
     }
  }
  colnames(est)<-prog
  rownames(est)<-index.tab[,1]

  save.as.tex.table(paste("advection",toupper(direction),".tex",sep=""),rownames(est),colnames(est),
                    est,"Region",fmt="%.1f")
# plot.diffusivity(est,std,uv.active)
  plot.UV(est,std,direction)
  return(list(est=est,std=std))
}


plot.UV<-function(est,std,direction)
{
   nregion<-length(rownames(est))
   yrange<-c(min(est-sd.mult*std,na.rm=T),max(est+sd.mult*std,na.rm=T))
   print(yrange)
   xrange<-c(1,nregion)
   x <- c(1:nregion)

   width <- 11.0
   height <- 6.0
   x11(width=width,height=height)

   def.par <- par(no.readonly = TRUE) 

   
   if (toupper(direction) == "U")
     plot(xrange,yrange,type='n',axes=FALSE, xlab="Region",ylab="Eastward Directed Movement (Nmi/Day)")
   else
     plot(xrange,yrange,type='n',axes=FALSE, xlab="Region",ylab="Northward Directed Movement (Nmi/Day)")

   nprog <- ncol(est)
   dx <-0.075

   for (prog in 1:nprog)
   {
     if (prog == 1)
       col <- "blue"
     else if (prog == 2)
       col = "lightblue"
     else if (prog >= 3)
       col = gray((prog-1)/nprog) #prog + 11

     abline(h=median(est[,prog],na.rm=TRUE),col=col,lwd=2,lty="dotdash")

      pch <- toupper(substr(colnames(est)[prog],1,1))
      for (rr in 1:nregion)
      {
        qq <- rr
        xx <- qq+(prog-2)*dx
        if (!is.na(est[qq,prog]))
        {
           plot.sd.range(xx,est[qq,prog],std[qq,prog],fcol=col)
        }
      }
   }
   axis(1,at=x,labels=rownames(est),cex.axis=0.8,lwd=0)
   axis(2,las=1,lwd=0)

   #box()
   abline(h=par("usr")[3],lwd=3)
   abline(v=par("usr")[1],lwd=3)

   ltext<-c("1977 - 1982       ","1989 - 1995       ","2006 - 2010")
   lcol<-c("blue","lightblue",gray(2/nprog))
   legend(x=nregion/2,y=yrange[2],legend=ltext,col=lcol,pch=20,pt.cex=3,
          xjust=0.0,yjust=0.5,horiz=TRUE,box.lty=0)


   save.png.plot(paste("advection",toupper(direction),sep=""),width,height)
   par(def.par)
}

M.tab<-function(uv.active=1,domain=FALSE)
{
  index.tab <- read.csv(paste(path,"GenRegion/RegionNumbering.csv",sep=""),header=TRUE)
  nregion<-nrow(index.tab)
  nmort <- length(mort.names)
  fits<-all.fits[(uv.active+1),]
  nfit <- length(fits)
  prog <- vector(length=nfit)
  par  <- vector(length=nfit)
  est <- matrix(nrow=nmort,ncol=nfit)
  std <- matrix(nrow=nmort,ncol=nfit)
  print(paste("Processing",nfit,"fits:"))
  for (ff in 1:nfit)
  {
     fit <- fits[ff]
     fitsplit <- unlist(strsplit(fit,"/"))
     prog[ff]<-fitsplit[1]
     par[ff] <- fitsplit[length(fitsplit)]
     print(paste(ff,prog[ff],par[ff]))
     file<-paste(path,sub(".","-",fit,fixed=TRUE),sep="")
     print(paste(ff,file))

     tmp <- read.admb.cor(file)
     par <- read.par(fit,path)
     par.F.ndx <- match(par$fleet.names,mort.names)
     print(par.F.ndx)
     print(mort.names[par.F.ndx])

     Z.names<-c("mort", "total_fishing_mortality","total_domain_mortality")
     wZ<-match(Z.names,tmp$names)
     print(wZ)
     print(tmp$names[wZ])
 
     if (domain)
        wF<-which(tmp$names=="domain_average_fishing_mortality")
     else
        wF<-which(tmp$names=="average_fishing_mortality")
     tmp.ndx <- c(wZ,wF)
     print(tmp.ndx)
     print(tmp$names[tmp.ndx])

     row.ndx <-c(1:3,par.F.ndx)
     print(row.ndx)
     print(mort.names[row.ndx])
     for (f in 1:nmort)
     {
        row <- row.ndx[f]
        ndx <- tmp.ndx[f]
        est[row,ff] <- tmp$est[ndx]
        std[row,ff] <- tmp$std[ndx]
     }
  }
  colnames(est)<-prog
  rownames(est)<-mort.names

  if (domain)
     save.as.tex.table(paste("mortality",uv.active,"d.tex",sep=""),mort.names, prog, est,"Fleet",fmt="%.5f")
  else
     save.as.tex.table(paste("mortality",uv.active,".tex",sep=""),mort.names, prog, est,"Fleet",fmt="%.5f")
  plot.mortality(est,std,uv.active,domain)
  return(list(est=est,std=std))
}

plot.mortality<-function(est,std,uv.active,domain)
{
   nmort <- length(mort.names)

   width <- 13.0
   height <- 6.0
   x11(width=width,height=height)

   def.par <- par(no.readonly = TRUE) 
   par(mar=(c(4,4,0,0)+0.1))
   nf <- layout(matrix(c(1:2),nrow=1,ncol=2),widths=c(5,nmort))
   layout.show(nf)

   xlab <- c(" ","Fleet")
   ylab <- c("Mortality (per month)"," ")
   nprog <- ncol(est)
   dx <-0.075

   print(rownames(est))

   for (np in 1:2)
   {
     if (np == 1)
       dat <- 1:3
     else
       dat <- 4:nmort

     yrange<-c(0,max(est[dat,]+sd.mult*std[dat,],na.rm=T))
     print(yrange)
     if (np == 1)
       xrange<-c(0,4)
     else
       xrange<-range(dat)
     x <- dat

     plot(xrange,yrange,type='n',axes=FALSE, xlab=xlab[np],ylab=ylab[np])

     for (prog in 1:nprog)
     {
       if (prog == 1)
         col <- "blue"
       else if (prog == 2)
         col = "lightblue"
       else if (prog >= 3)
         col = gray((prog-1)/nprog) #prog + 11

        for (rr in dat)
        {
          xx <- rr-(nprog-(prog*2))*dx
          if (!is.na(est[rr,prog]))
          {
          #  double.lines(c(xx,xx), c((est[rr,prog]-sd.mult*std[rr,prog]),(est[rr,prog]+sd.mult*std[rr,prog])),
          #        fcol=col,lwd=11)
          #  points(xx, est[rr,prog], col="white", pch=20) #pch="_",cex=2)
             plot.sd.range(xx,est[rr,prog],std[rr,prog],fcol=col)
          }
        }
     }
     axis(1,at=x,labels=rownames(est)[dat],cex.axis=0.8,lwd=0)
     axis(2,las=1,lwd=0)
     abline(h=par("usr")[3],lwd=3)
     abline(v=par("usr")[1],lwd=3)

     if (np == 2)
     {
        ltext<-c("1977 - 1982","1989 - 1995","2006 - 2010")
        lcol<-c("blue","lightblue",gray(2/nprog))
        legend(x=xrange[2],y=yrange[2],legend=ltext,col=lcol,pch=20,pt.cex=3,
               xjust=1.0,yjust=0.5,horiz=TRUE,box.lty=0)
     }
   # box()
   }
   if (domain)
      save.png.plot(paste("mortality-change",uv.active,"d",sep=""),width,height)
   else
      save.png.plot(paste("mortality-change",uv.active,sep=""),width,height)
   par(def.par)
}

rr.plot<-function(fit="gen2.p05")
{
  #~/SPC2011/pttp/OneDegree/Gen/skjq/gen2.p05
  total.caps<-c(197, 1468, 160, 4311, 1360, 2816, 233, 425, 1030)
  stc<-sort(total.caps, index.return=TRUE)
  ndx<-stc$ix
  print(ndx)
  
  print(paste("reading par file",fit))
  par <- read.par(fit)
  print(paste("reading admb files",fit))
  admb<-read.admb.files(fit)
  rr<-which(admb$names == "report_rates")
  std<-2*admb$std[rr]
  std<-c(std[ndx],0.12,0.18)
  est<- admb$est[rr]
  est<-c(est[ndx],0.53,0.45)
  names <- c(par$fleet.names[ndx],"M2","M3")
  print(length(names))
  print(names)
  yrange <- c(min(est-sd.mult*std),max(est+sd.mult*std))

  x11(width=9.0,height=4.5)
  plot(c(1,length(names)),yrange,type='n',axes=FALSE,
        xlab="Purse Seine Fleet and MFCL Regions",ylab="Reporting Rate")
  for (n in 1:length(names))
  {
  # double.lines(c(n,n),c(est[n]-sd.mult*std[n],est[n]+sd.mult*std[n]),
  #              fcol="lightblue",lwd=11)
  # points(n,est[n],col="white",pch=20,cex=1)
    plot.sd.range(n,est[n],std[n],fcol="lightblue")
  }
  min.M<-min(est[length(names)-1]-sd.mult*std[length(names)-1],est[length(names)]-sd.mult*std[length(names)])
  max.M<-max(est[length(names)-1]+sd.mult*std[length(names)-1],est[length(names)]+sd.mult*std[length(names)])
  abline(h=min.M,col="gray")
  abline(h=max.M,col="gray")

  axis(1,at=seq(1,length(names)),labels=names,lwd=0)
  axis(2,las=1,lwd=0)
  abline(h=par("usr")[3],lwd=3)
  abline(v=par("usr")[1],lwd=3)

  save.png.plot(paste("rr-",fit,sep=""),width=9.0,height=4.5,gpath="./")
}
#~/SPC2011/pttp/OneDegree/Gen/skjq/tagmove.log
#~/SPC2011/pttp/OneDegree/Gen/skjq/gen2.p05
#Total recaptures by fleet:
#        fleet    W/O      W    sum
#  1      fmps     26    171    197
#  2      jpps    133   1335   1468
#  3      krps     24    136    160
#  4      pgps    135   4176   4311
#  5      phps    281   1079   1360
#  6      sbps    270   2546   2816
#  7      twps     70    163    233
#  8      usps     79    346    425
#  9      vups    781    249   1030


save.png.plot<-function(root,width=6.5,height=4.5,gpath="./graphics/")
{
# graphics.root <-paste("./graphics/",root,sep="")
  graphics.root <-paste(gpath,root,sep="")
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

plot.sd.range<-function(x, est, sd, mult=2.0, fcol,bcol="black",lwd=11)
{
   double.lines(c(x,x),c((est-mult*sd),(est+mult*sd)),bcol=bcol,fcol=fcol,lwd=lwd)
   points(x,est,col="white",pch=20)
}

save.as.tex.table<-function(file,rowname,colname,value,mytext="mytext",fmt="%.0f")
{
  fileout<-paste("./tables/",file,sep="")
  message("Writing the table to the file ", fileout)
  nrow <- nrow(value)
  ncol <- ncol(value)

  cat("\\begin{tabular}{",file=fileout)
  # add one for row and column names
  for (j in 1:(ncol+1))
    cat("c",file=fileout, append=TRUE)
  cat("}\n\\hline\n",file=fileout,append=TRUE)


  cat(mytext, file=fileout, append=TRUE)
  for (j in 1:ncol)
    cat(paste(" &",colname[j]), file=fileout, append=TRUE)
  cat("\\\\\n\\hline\n", file=fileout, append=TRUE)

  for (i in 1:nrow)
  {
    cat(rowname[i], file=fileout, append=TRUE)
    for ( j in 1:ncol)
      if (is.numeric(value[i,j]))
        cat(paste(" &",sprintf(fmt,value[i,j])), file=fileout, append=TRUE)
      else
        cat(paste(" &",value[i,j]), file=fileout, append=TRUE)

    cat("\\\\\n", file=fileout, append=TRUE)
  }



  cat("\\hline\n\\end{tabular}",file=fileout,append=TRUE)
}
 
