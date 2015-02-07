require("movemodutils")
require("treemap")
paste0<-function(...){paste(...,sep="")}
#DefaultRamp <-colorRampPalette(c("#0080FF","#FF8000"),bias=1.0)
#ramp <- DefaultRamp
#pal <- ramp(as.integer(yrange[2]-yrange[1]+1))
   

global.fit.tab<-function()
{
  #  also matches .pdf
  #  pattern = "*\\.p[0-9,a-f]{2,}$"
  # 
  pattern = "*.p[0-9,a-f][0-9,a-f][0-9,a-f]$"
  print(pattern)
  files<-dir(".", pattern = pattern,recursive=TRUE)

  print(files)
  nfile <- length(files)
  nfit <- nfile
  root = "X"
  splitff <- lapply(files,strsplit,"[\\/\\.]")
  splitff <- lapply(splitff,unlist)

  pars<-lapply(files,read.par)
  cnames <- c("path","dir","root","fit","res","crit","steps","pool","uv","q","R","fleets","DO","seasons","m1","s11","s51","s53","M","n","logL","maxG","nfe")
  print(cnames)
  rnames<-vector(length=nfit)

  #for (pass in 1:2)
  pass <- 1
  {
    print(paste("pass",pass))
    ncol <- length(cnames)
    fits <- data.frame(ncol=length(cnames),nrow=nfit,stringsAsFactors=FALSE)
    rnames[1:nfit]<-NA
    f <- 0
    for (ff in 1:nfit)
    {
      par <- pars[ff][[1]]
      if (pass == 1 ||
          (par$flags[3]  < 10 &&
           par$flags[6]  == 6 &&
           par$flags[21] == 2 &&
       #   par$flags[22] == 1 &&
           par$flags[23] == 1 &&
           par$flags[25] == 2 &&
           par$maximum.gradient.magnitude <= 0.1) )
      {
           f <- f+1
           do <- 0
           for (g in 1:par$nfleet)
           {
             do <- do + par$do.fleet.kludge[g]
           }
           file.parts <- unlist(strsplit(files[ff],"[\\/\\.]"))
           lfp <- length(file.parts)
           c <- 0
           c<- c+1; fits[f,c] <- files[ff]
           c<- c+1; fits[f,c] <- splitff[[ff]][lfp-2]
           c<- c+1; fits[f,c] <- splitff[[ff]][lfp-1]
           c<- c+1; fits[f,c] <- splitff[[ff]][lfp]
           c<- c+1; fits[f,c] <- par$deltax
           c<- c+1; fits[f,c] <- par$flags[3]
           c<- c+1; fits[f,c] <- par$flags[6]
           c<- c+1; fits[f,c] <- par$flags[13]
           c<- c+1; fits[f,c] <- par$flags[23]
           c<- c+1; fits[f,c] <- par$flags[25]
           c<- c+1; fits[f,c] <- par$flags[28]
           c<- c+1; fits[f,c] <- par$nfleet
           c<- c+1; fits[f,c] <- do
           c<- c+1; fits[f,c] <- par$nseason
           c<- c+1; fits[f,c] <- par$start.month[1]
           c<- c+1; fits[f,c] <- par$flags[11]
           c<- c+1; fits[f,c] <- par$flags[51]
           c<- c+1; fits[f,c] <- par$flags[53]
           c<- c+1; fits[f,c] <- par$natural.mortality
           c<- c+1; fits[f,c] <- par$number.of.variables
           c<- c+1; fits[f,c] <- par$objective.function.value
           c<- c+1; 
              if(par$maximum.gradient.magnitude > 0.0)
                 fits[f,c] <- log10(par$maximum.gradient.magnitude)
              else
                 fits[f,c] <- 10.0
           c<- c+1; fits[f,c] <- par$flags[5]
           rnames[f] <- files[ff]
       }
       colnames(fits) <- cnames;
    #  rownames(fits) <- rnames;
    }

    if (pass == 2)
    {
      w<-which(!is.na(rnames))
      write.csv(fits[w,],paste(root,"X_good_fit_tab.csv",sep=""),quote=FALSE)
    }
    else
    {
      write.csv(fits,paste(root,"X_fit_tab.csv",sep=""),quote=FALSE,row.names=FALSE)
    }
  }

# treemap(fits, index = c("res","seasons"), vSize = "M", vColor = "logL", palette=terrain.colors(10))
  treemap(fits, index = c("res","steps","seasons","pool","fleets","q"), vSize = "res",type="value",vColor="M",fontsize.labels=14)
  x11()
  treemap(fits, index = c("res","steps","seasons","pool","fleets","q"), vSize = "res",type="value",vColor="maxG",fontsize.labels=14,lowerbound.cex.labels=.6)

  return(fits)
}
