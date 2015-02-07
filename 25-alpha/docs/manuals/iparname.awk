BEGIN{OFS=""
buffer = "%"
print buffer > "iparname.tex"
buffer = buffer"%"
}
{
  pos = match($0,"set_name")
  if (pos == 5)
  {
  #  gsub("\\^","$^$",buffer)
     gsub("D\\^2","$D^2$",buffer)
     gsub("&","and",buffer)
     gsub("_","\\_",buffer)
     sub(/;$/,"",buffer)
     buffer = buffer"."
     print buffer >> "iparname.tex"
     split($0,a,"\"")
     text = a[2]
     gsub(/=>/," $\\Rightarrow$ ",text)
     split(a[1],b,"(")
     flag = b[2]
     sub(",","",flag)
     gsub(" ","",flag)

     buffer =  "\\item[Flag "flag"] "text
  }
  cpos = match($0,/^ *\"/)
  if (cpos > 0)
  {
    print gsub("\"","",$0)
    print gsub("  ","",$0)
    buffer = buffer" "$0

  }
  dpos = match($0,/^ *t\[/)
  if (dpos > 0)
  {
     buffer = buffer"; default = "$3
  }
}
END{
# gsub("\\^","$^$",buffer)
  gsub("&","and",buffer)
  gsub("_","\\_",buffer)
  sub(/;$/,"",buffer)
  print buffer >> "iparname.tex"
  print "%%%" >> "iparname.tex"
}

