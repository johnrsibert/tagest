BEGIN{FS="/| *"
SUBSEP=","
tcaps=0
trels=0
}
{
  if (strtonum($1) > 0)
  {
#   print $3,$4,$5
    trels = trels + $5
    rlon = lon_field($4)
    rlat = lat_field($3)
    release[rlon,rlat] = release[rlon,rlat] + $5
  }
  else
  {
  # print NF,": ",$0
  # print $5,$6
    if ( ($5 != "NA") && ($6 != "NA"))
    {
      tcaps = tcaps + 1
      clon = lon_field($6)
  #   print $6,clon
      clat = lat_field($5)
  #   print $5,clat
      recaps[clon,clat] = recaps[clon,clat] + 1
    }
  }
}
END{
  print "total releases = ",trels
  for (rpos in release)
     print rpos,",",release[rpos] > "rels.xyz"
 
  print "\ntotal recaptures = ",tcaps
  for (cpos in recaps)
     print cpos,",",recaps[cpos] > "caps.xyz"
}
 

function lon_field(field)
{
  tmp = field
  sub("E","",tmp)
  sub("W","",tmp)
  nc = length(field)
  if (substr(field,nc) == "W")
    tmp = -tmp
  return(tmp)
}
 
function lat_field(field)
{
  tmp = field
  sub("S","",tmp)
  sub("N","",tmp)
  nc = length(field)
  if (substr(field,nc) == "S")
    tmp = -tmp
  return(tmp)
}
