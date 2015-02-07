require("movemodutils")
fit.tab<-function(root)
{
  print(root)
  pattern = paste(root,"\\.p[0-9,a-f][0-9,a-f][0-9,a-f]",sep="")
  print(pattern)

  files<-dir(".", pattern = pattern,recursive=TRUE)
  if (root =="*")
    root ="global"

  print(files)
  print(length(files))

# fits<-lapply(files,read.par)
# ret<-list()
# ret$file <- files
# ret$fit<-fits

  pars<-lapply(files,read.par)
  nfit <- length(pars)
  print(nfit)
  cnames <- c("crit","steps","pool","uv","q","R","fleets","DO","seasons","m1","s11","s51","s53","M","n","-log L","max |g|","nfe")
  print(cnames)
  rnames<-vector(length=nfit)

  for (pass in 1:2)
  {
    print(paste("pass",pass))
    ncol <- length(cnames)
    fits <- matrix(ncol=length(cnames),nrow=nfit)
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
           c <- 0
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$flags[3])
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$flags[6])
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$flags[13])
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$flags[23])
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$flags[25])
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$flags[28])
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$nfleet)
           c<- c+1; fits[f,c] <- sprintf(" %3i",do)
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$nseason)
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$start.month[1])
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$flags[11])
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$flags[51])
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$flags[53])
           c<- c+1; fits[f,c] <- sprintf(" %.3f",par$natural.mortality)
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$number.of.variables)
           c<- c+1; fits[f,c] <- sprintf(" %.2f",par$objective.function.value)
           c<- c+1; fits[f,c] <- sprintf(" %.3g",par$maximum.gradient.magnitude)
           c<- c+1; fits[f,c] <- sprintf(" %3i",par$flags[5])
           rnames[f] <- files[ff]
       }
       colnames(fits) <- cnames;
       rownames(fits) <- rnames;
    }

    if (pass == 2)
    {
      w<-which(!is.na(rnames))
      print(w)
      write.csv(fits[w,],paste(root,"_good_fit_tab.csv",sep=""),quote=FALSE)
      write.table(fits[w,], file = paste(root,"_good_fit_tab.tex",sep=""), quote = FALSE, sep = " & ",
                 eol = "\\\\\n")
    }
    else
    {
      write.csv(fits,paste(root,"_fit_tab.csv",sep=""),quote=FALSE)
      write.table(fits, file = paste(root,"_fit_tab.tex",sep=""), quote = FALSE, sep = " & ",
                 eol = "\\\\\n")
    }
  }
   
}
