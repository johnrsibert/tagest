#plot.recaps.by.fleet.1<-function(file="recaps_by_fleet.csv")
#{
#   recaps<-read.csv(file=file, header=TRUE)
#   index<-sort.int(recaps$total,index.return=TRUE,decreasing=TRUE)
#   ndx <- index$ix
#
#   nfleet <- length(ndx)
#   x <-seq(1:nfleet)
#   ctotal<-recaps[ndx,]$total
#   fnames<-recaps[ndx,]$fleet
#   print(ctotal)
#   for (f in 2:nfleet)
#   {
#      ctotal[f] = ctotal[f]+ctotal[f-1]
#   }
#   prop=ctotal
#   for (f in 1:nfleet)
#   {
#     prop[f]=prop[f]/prop[nfleet]
#   }
# 
#   print(ctotal)
#   plot(x,ctotal,type='b',pch=3,lwd=3,col="blue",axes=FALSE,xlab="",
#     ylim=c(0,max(ctotal)),ylab="Cumulative Recaptures")
#   axis(1,at=x,labels=fnames,las=2,cex=0.6)
#   axis(2)
#   box()
#
#   ret<-cbind(x,recaps[ndx,],ctotal,prop)
#   return(ret)
#}

#plot.recaps.by.fleet<-function(recaps)
plot.recaps.by.fleet<-function(tag.files=
                      c("sibert_recovery_all_table_SKJ_small_PTTP_1deg.txt",
                        "sibert_recovery_all_table_SKJ_big_PTTP_1deg.txt"))
{
   recaps <- as.data.frame(read.recaps(tag.files))
   print(head(recaps))

   index<-sort.int(recaps$total,index.return=TRUE,decreasing=TRUE)
   ndx <- index$ix
   print(ndx)

   nfleet <- length(ndx)
   x <-seq(1,nfleet)
#  ctotal<-recaps[ndx,]$total
   total <-recaps[ndx,]$total
   ctotal<-vector(length=nfleet)
#  fnames<-recaps[ndx,]$fleet
   fnames<-rownames(recaps)
   ctotal[1] <- total[1]
   for (f in 2:nfleet)
   {
      ctotal[f] = ctotal[f-1]+total[f]
   }

   prop <- total
   for (f in 1:nfleet)
   {
     prop[f] <- prop[f]/ctotal[nfleet]
   }

   cprop <- prop
   for (f in 2:nfleet)
   {
      cprop[f] = cprop[f-1]+prop[f]
   }

   x11(width=12,height=4.0)
   plot(x,ctotal,type='n',axes=FALSE,xlab="",
      ylim=c(0,max(ctotal)),ylab="Cumulative Recaptures")

   lwd <- 3
   col <- "blue"
   for (f in 1:nfleet)
   {
   #  points(x[f],ctotal[f])
      if (f == 1)
         lines(c(x[f],x[f]),c(0,ctotal[f]),col=col,lwd=lwd)
      else
      {
         lines(c(x[f-1],x[f]),c(ctotal[f-1],ctotal[f-1]),col=col,lwd=lwd)
         lines(c(x[f],x[f]),c(ctotal[f-1],ctotal[f]),col=col,lwd=lwd)
      } 
   }
   axis(1,at=x,labels=fnames[ndx],las=2,cex=0.6)
   axis(2)
   box()

   froot <- "recaps_by_fleet"
   save.png.plot(froot,width=18,height=6,gpath="./")
   
   ret<-cbind(x,recaps[ndx,],ctotal,prop,cprop)
   fmt<-c("%d","%d","%d","%d","%d","%1.3f","%1.3f")
   save.as.tex.table(paste(froot,".tex",sep=""),
     rownames(ret),colnames(ret),ret,"Fleet",fmt)
   return(ret)
}

#read.recaps<-function(tag.files=c("sibert_recovery_all_table_SKJ_small_PTTP_1deg.txt",
#                                  "sibert_recovery_all_table_SKJ_big_PTTP_1deg.txt"))
read.recaps<-function(tag.files)
{
   maxfleet <- 33000
   fleets <- vector(length=maxfleet)
   nfile = length(tag.files)
   n <- 0
   k <- 0
   for (f in 1:nfile)
   { 
      lines<-readLines(con=tag.files[f])
      nline <- length(lines)
      print (paste(f,tag.files[f],nline))
      n <- n + nline
      if (n >= maxfleet)
      {
          print("need to increase maxfleet")
          exit
      }
   
      for (i in 1:nline)
      {
         k <- k + 1
         fleets[k] <- get.fleet.name(lines[i])
      }
   }
   print(head(fleets))
   w <- which(fleets!=FALSE)
   ufleets<-unique(fleets[w])
   print(paste("Found",k,"fleet names,",length(ufleets),"are unique"))
   print(ufleets)

   recaps.by.fleet<-matrix(0,nrow=length(ufleets),ncol=3)
   colnames(recaps.by.fleet)<-c("small","big","total")
   rownames(recaps.by.fleet)<-ufleets
   n<-0
   for (f in 1:nfile)
   { 
      lines<-readLines(con=tag.files[f])
      nline <- length(lines)
      print (paste(f,tag.files[f],nline))
      n <- n + nline
      if (n >= maxfleet)
      {
          print("need to increase maxfleet")
          stop()
      }
   
      for (i in 1:nline)
      {
         fleet <- get.fleet.name(lines[i])
         k <- match(fleet,ufleets,nomatch=0)
      #  print(paste(fleet,k))
         if (nfile > 1)
           recaps.by.fleet[k,f] <- recaps.by.fleet[k,f] + 1
         recaps.by.fleet[k,3] <- recaps.by.fleet[k,3] + 1
      }
   }


   #froot <- "recaps_by_fleet"
   #write.csv(recaps.by.fleet,file="recaps_by_fleet.csv")
   #fmt<-c("%d","%d","%d","%d","%d","%1.2f")
   #save.as.tex.table(paste(froot,".tex",sep=""),
     #rownames(recaps.by.fleet),colnames(recaps.by.fleet),recaps.by.fleet,"Fleet",fmt)

   return(recaps.by.fleet)
}

get.fleet.name<-function(line)
{
    tmp <- unlist(strsplit(line," "))
#   print(paste(i,length(tmp),tmp[1],tmp[2]))
    if ( (length(tmp) == 29) &&
          (tmp[28]!="") && (tmp[29]!="") )
    {
        fleet<-toupper(paste(tmp[29],tmp[28],sep=""))
    }
    else
    {
        fleet = NA
    }
    return(fleet) 
}

############################################

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

save.as.tex.table<-function(file,rowname,colname,value,mytext="mytext",fmt)
{
  fileout<-file #paste("./tables/",file,sep="")
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
    {
        cat(paste(" &",sprintf(fmt[j],value[i,j])), file=fileout, append=TRUE)
    }
    cat("\\\\\n", file=fileout, append=TRUE)
  }



  cat("\\hline\n\\end{tabular}",file=fileout,append=TRUE)
}

