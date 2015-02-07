<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!-- $Id: ADMB.xsl 2938 2012-02-01 00:45:52Z jsibert $ -->
<xsl:template match="/">
  <html>
  <body>
  <center>
  <h1>ADMB parameter file: <xsl:value-of select="FIT/@id"/> </h1>
  </center>
  <h2>ADMB Model Parameters</h2>
  <table border="1">
    <tr bgcolor="#9acd32">
      <th>Parameter</th>
      <th>Variable Name</th>
      <th>Value</th>
      <th>Bounds</th>
      <th>Phase</th>
      <th>Active</th>
    </tr>
  <xsl:for-each select="FIT/*[@category='VAR']">
  <tr>
      <td><xsl:value-of select="title"/></td>
      <td><xsl:value-of select="local-name()"/></td>
      <td><xsl:value-of select="value"/></td>
      <td><xsl:value-of select="bounds/min"/> to 
          <xsl:value-of select="bounds/max"/></td>
      <xsl:choose>
         <xsl:when test="phase &gt; 0">
           <td align="center">
           <font color="#ff0000">
           <xsl:value-of select="phase"/>
           </font></td>
         </xsl:when>
         <xsl:otherwise>
           <td align="center"><xsl:value-of select="phase"/></td>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:choose>
         <xsl:when test="active &gt; 0">
           <td align="center">
           <font color="#ff0000">
           <xsl:value-of select="active"/>
           </font></td>
         </xsl:when>
         <xsl:otherwise>
           <td align="center"><xsl:value-of select="active"/></td>
         </xsl:otherwise>
      </xsl:choose>
   </tr>
  </xsl:for-each>
  </table>

  <h2>ADMB Model Data</h2>
  <table border="1">
    <tr bgcolor="#9acd32">
      <th>Data&#160;Element</th>
      <th>Variable&#160;Name</th>
      <th>Value</th>
    </tr>
  <xsl:for-each select="FIT/*[@category='CON']">
  <tr>
      <td><xsl:value-of select="title"/></td>
      <td><xsl:value-of select="local-name()"/></td>
      <td><xsl:value-of select="value"/></td>
   </tr>
  </xsl:for-each>
  </table>

  <h2>movemod tag release cohorts (pooled)</h2>
  <table border="1">
    <tr bgcolor="#9acd32">
      <th>Cohort</th>
      <th>Release Date</th>
      <th>Longtitude</th>
      <th>Latitude</th>
      <th>Number Released</th>
    </tr>
  <xsl:for-each select="FIT/releases/cohort">
  <tr>
      <td><xsl:value-of select="number"/></td>
      <td><xsl:value-of select="date"/></td>
      <td><xsl:value-of select="longitude"/></td>
      <td><xsl:value-of select="latitude"/></td>
      <td><xsl:value-of select="tags"/></td>
   </tr>
  </xsl:for-each>
  </table>

  <h2>movemod ipars</h2>
  <table border="1">
    <tr bgcolor="#9acd32">
      <th>Index</th>
      <th>Flag Name</th>
      <th>Value</th>
    </tr>
  <xsl:for-each select="FIT/m_ipar/flag">
  <tr>
      <td align="center"><xsl:value-of select="index"/></td>
      <td><xsl:value-of select="name"/></td>
      <td align="right"><xsl:value-of select="value"/></td>
   </tr>
  </xsl:for-each>
  </table>

  </body>
  </html>
</xsl:template>

</xsl:stylesheet> 
