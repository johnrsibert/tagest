htest<-function(LL1,n1,LL2,n2)
{
  if ( n2 == n1)
  {
    print("Error n1 = n2")
    return(NULL)
  }
  else if (n2 > n1)
  {
    t  <- n1
    n1 <- n2
    n2 <- t
    t  <- LL1
    LL1 <- LL2
    LL2 <- t
    print("Warning: (LL2,n2) exchanged for (LL1,n1)")
  }
    
  print("",quote=F)
  print(sprintf("%s","Results of hypothesis testing for"),quote=F)
  print(sprintf("   %s: log L = %.3f, n = %i","H1",LL1,as.integer(n1)),quote=F)
  print(sprintf("   %s: log L = %.3f, n = %i","H2",LL2,as.integer(n2)),quote=F)

  df = n1 -n2
  
  chi2 = 2.0*(LL1-LL2)
  P <- pchisq(q=chi2,df=df)
  print(sprintf("Likelihood ratio test: chi^2 = %.3f with %i degrees of freedom; P = %.3f",
              chi2,as.integer(df),P),quote=F)

  LA12 <- LL2 - LL1 +0.5*df*log(2)
  A12 <- exp(LA12) 
  if (A12 < 0.001)
    evidence <-"overwhelming"
  else if (A12 < 0.01)
    evidence <- "very strong"
  else if (A12 < 0.05)
    evidence <- "strong"
  else
    evidence <- "nil"

  print(sprintf("Posterior Bayes factor = %.3e; sample evidence is %s",A12,evidence),quote=F)

# AIC = -2 ln L +2p and BIC = -2 ln L + p ln(n) 
# p = number of parameters
# n = number of observations

  htest.results<-list(df=df,LR.chi2=chi2,LR.P=P,PBF.A=A12,PBF.evidence=evidence)
  return(htest.results)
  
}
