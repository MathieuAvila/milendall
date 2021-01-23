<?xml version="1.0"?>
<xsl:stylesheet xmlns="http://www.w3.org/1999/xhtml" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:output method="text" encoding="utf-8"/>

  <xsl:template match="*" priority="-1000">
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="maze">
digraph g { 
  node [shape=box];

<xsl:for-each select="room">"<xsl:value-of select="@id"/>" [ label="<xsl:value-of select="@id"/><xsl:if test="name">&#xa;'<xsl:value-of select="name/text()"/>' </xsl:if>" ];
<xsl:for-each select="portal">
      "<xsl:value-of select="../@id" />" -> "<xsl:value-of select="@to" />" [
      label = "<xsl:value-of select="@id" />"
      <xsl:if test="not(@bidirectional = 'false')">dir="both"</xsl:if>
      ];
</xsl:for-each>

</xsl:for-each>
}
</xsl:template>

</xsl:stylesheet>
