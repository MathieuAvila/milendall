
Vizualize maze graphically
==========================

Example to generate one graphviz output


.. code-block:: shell

    xsltproc generator/rooms_graph.xsl2 generator/samples/simple.xml > graph_test.graph 
    cat graph_test.graph 
    dot -Tpng graph_test.graph -otest.png



