getRandomInt = function(min, max) {
   return Math.floor(Math.random() * (max - min + 1)) + min;
   }

addRandomNode = function(){
   idNum = cy.nodes().size(),
   setID = idNum.toString(),
   cy.add([{group: "nodes",
            data: {"id": setID,
                   "name": "added",
                   "resources": [],
                   "properties": []
                   },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                },
             }]); // cy.add
   }; // addRandomNode

addNodeWithID = function(idstr){
   //idNum = cy.nodes().size(),
   //setID = idNum.toString(),
   //setID = idstr,
   cy.add([{group: "nodes",
            data: {"id": idstr,
                   "label": idstr,
                   "resources": [],
                   "properties": []
                   },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                },
             }]); // cy.add
   }; // addRandomNode

addMbFSM = function(idstr){
  var model = idstr.split("_");
  var i = 0;
  //if ((model[0].toString()).equals("FW")){
  alert(model[0]);
  // FW model  
  if (model[0] == "FW"){
    //alert("FW");
    cy.add([{group: "nodes",
      "data" : {
        "id" : idstr+"NULL",
        "parent" : idstr,
        "label" : "NULL",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"SYN_RECEIVED",
        "parent" : idstr,
        "label" : "SYN_RECEIVED",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"INVAL",
        "parent" : idstr,
        "label" : "INVAL",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"NULL"+idstr+"INVAL",
        "label" : "*-SYN",
        "source" : idstr+"NULL",
        "target" : idstr+"INVAL",
      }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"NULL"+idstr+"SYN_RECEIVED",
        "label" : "SYN",
        "source" : idstr+"NULL",
        "target" : idstr+"SYN_RECEIVED",
      }
    }
    ]); // cy.add
  }

  // IPS model  
  if (model[0] == "IPS"){
    //alert("FW");
    cy.add([{group: "nodes",
      "data" : {
        "id" : idstr+"NULL",
        "parent" : idstr,
        "label" : "NULL",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"SYN_RECEIVED",
        "parent" : idstr,
        "label" : "SYN_RECEIVED",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"NULL"+idstr+"SYN_RECEIVED",
        "label" : "SYN",
        "source" : idstr+"NULL",
        "target" : idstr+"SYN_RECEIVED",
      }
    }
    ]); // cy.add
  }

// Proxy model  
  if (model[0] == "Proxy"){
    //alert("FW");
    cy.add([{group: "nodes",
      "data" : {
        "id" : idstr+"NULL",
        "parent" : idstr,
        "label" : "NULL",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"INVAL",
        "parent" : idstr,
        "label" : "INVAL",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"NULL"+idstr+"INVAL",
        "label" : "*-SYN",
        "source" : idstr+"NULL",
        "target" : idstr+"INVAL",
      }
    }
    ]); // cy.add
  }

  // BohateiSYNIPS Model
  if (model[0] == "BohateiSYNIPS"){
    //alert("FW");
    cy.add([{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount=SYNCount",
        "parent" : idstr,
        "label" : "DiffCount=SYNCount",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount>0",
        "parent" : idstr,
        "label" : "DiffCount>0",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount=0",
        "parent" : idstr,
        "label" : "DiffCount=0",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"DiffCount=0"+idstr+"DiffCount>0",
        "label" : "SYN",
        "source" : idstr+"DiffCount=0",
        "target" : idstr+"DiffCount>0",
      }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"DiffCount=SYNCount"+idstr+"DiffCount>0",
        "label" : "ACK",
        "source" : idstr+"DiffCount=SYNCount",
        "target" : idstr+"DiffCount>0",
      }
    }
    ]); // cy.add
  }

  // BohateiSYNIPS Model
  if (model[0] == "CountingIPS"){
    //alert("FW");
    cy.add([{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount=SYNCount",
        "parent" : idstr,
        "label" : "attack",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount>0",
        "parent" : idstr,
        "label" : "unknown",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount=0",
        "parent" : idstr,
        "label" : "benign",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"DiffCount=0"+idstr+"DiffCount>0",
        "label" : "SYN",
        "source" : idstr+"DiffCount=0",
        "target" : idstr+"DiffCount>0",
      }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"DiffCount=SYNCount"+idstr+"DiffCount>0",
        "label" : "ACK",
        "source" : idstr+"DiffCount=SYNCount",
        "target" : idstr+"DiffCount>0",
      }
    }
    ]); // cy.add
  }

    // BohateiSYNIPS Model
  if (model[0] == "SYNProxy"){
    //alert("FW");
    cy.add([{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount=SYNCount",
        "parent" : idstr,
        "label" : "attack",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount>0",
        "parent" : idstr,
        "label" : "unknown",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount=0",
        "parent" : idstr,
        "label" : "benign",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"DiffCount=0"+idstr+"DiffCount>0",
        "label" : "tcp completed",
        "source" : idstr+"DiffCount>0",
        "target" : idstr+"DiffCount=0",
      }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"DiffCount=SYNCount"+idstr+"DiffCount>0",
        "label" : "!tcp completed",
        "source" : idstr+"DiffCount>0",
        "target" : idstr+"DiffCount=SYNCount",
      }
    }
    ]); // cy.add
  }

  if (model[0] == "LightIPS"){
    //alert("FW");
    cy.add([{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount>0",
        "parent" : idstr,
        "label" : "bad_conn>=10",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount=0",
        "parent" : idstr,
        "label" : "!(bad_conn>=10)",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"DiffCount=0"+idstr+"DiffCount>0",
        "label" : "bad_conn>=10",
        "source" : idstr+"DiffCount=0",
        "target" : idstr+"DiffCount>0",
      }
    }
    ]); // cy.add
  }

  if (model[0] == "HeavyIPS"){
    //alert("FW");
    cy.add([{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount>0",
        "parent" : idstr,
        "label" : "bad_signature",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    },{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount=0",
        "parent" : idstr,
        "label" : "!bad_signature",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    }, {group: "edges",
      "data" : {
        "id" : idstr+"DiffCount=0"+idstr+"DiffCount>0",
        "label" : "bad_signature",
        "source" : idstr+"DiffCount=0",
        "target" : idstr+"DiffCount>0",
      }
    }
    ]); // cy.add
  }

  // BohateiSYNIPS Model
  if (model[0] == "Allow"){
    //alert("FW");
    cy.add([{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount=SYNCount",
        "parent" : idstr,
        //"label" : "Allow",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    }
    ]); // cy.add
  }

// BohateiSYNIPS Model
  if (model[0] == "Block"){
    //alert("FW");
    cy.add([{group: "nodes",
      "data" : {
        "id" : idstr+"DiffCount=SYNCount",
        "parent" : idstr,
        //"label" : "Block",
      },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                }
    }
    ]); // cy.add
  }

};

addNodeWithFSM = function(idstr){
   //idNum = cy.nodes().size(),
   //setID = idNum.toString(),
   //setID = idstr,
   /*
   cy.add([{group: "nodes",
            data: {"id": idstr,
                   "resources": [],
                   "properties": []
                   },
             position: {
                x: getRandomInt(100,800),
                y: getRandomInt(100,500),
                },
             }]); // cy.add
   */

   addNodeWithID(idstr);

   // add tiny FSM
   addMbFSM(idstr);

}; // addRandomNode

addRandomEdge = function(){
   nodeCount = cy.nodes().size()
   edgeCount = cy.edges().size()
   idNum = edgeCount
   setID = idNum.toString(),
   sourceNode = getRandomInt(0, nodeCount-1)
   targetNode = getRandomInt(0, nodeCount-1)
   cy.add([{group: "edges",
            data: {"id": "e" + setID,
                   "source": sourceNode,
                   "target": targetNode
                   }
             }]); // cy.add
  }; // addRandomEdge

addEdgeWithID = function(srcID, dstID, edgeID, context){
   //nodeCount = cy.nodes().size()
   //edgeCount = cy.edges().size()
   cy.add([{group: "edges",
            data: {"id": edgeID,
                   "source": srcID,
                   "target": dstID,
                   "label": context
                   }
             }]); // cy.add
  }; // addRandomEdge

modLabel = function(IDstr, val){
   //nodeCount = cy.nodes().size()
   //edgeCount = cy.edges().size()
   //cy.getElementById(IDstr).css({'label': val});
   cy.getElementById(IDstr).css({'content': val});
   //cy.$(IDstr).
   //alert(IDstr+val);
}; // addRandomEdge

readMultipleFiles = function (evt) {
    //Retrieve all the files from the FileList object
            var r = new FileReader();
            r.onload = (function (f) {
                return function (e) {
                    var contents = e.target.result;
                    alert(contents);
                };
            })(f);
            r.readAsText(f);
}

fitGraph = function() {
  cy.fit();
  cy.forceRender()
  } // fitGraph


freshLayout = function(){

   coseOptions = {
       name: 'cose',
       ready               : function() {},
       stop                : function() {},
       refresh             : 0,
       fit                 : true, 
       padding             : 30, 
       randomize           : true,
       debug               : false,
       nodeRepulsion       : 10000,
       nodeOverlap         : 10,
       idealEdgeLength     : 10,
       edgeElasticity      : 100,
       nestingFactor       : 5, 
       gravity             : 250, 
       numIter             : 100,
       initialTemp         : 200,
       coolingFactor       : 0.95, 
       minTemp             : 1
       };

   cy.layout(coseOptions);
   cy.fit()
   } // freshLayout

$(document).ready(function() {
   console.log("document ready");
   var $cy = $("#cy");
   $cy.cytoscape({
       //elements: network.elements,
       //style: vizmap[0].style,  // the first style is the one we want

    style: [
    {
      selector: 'node',
      css: {
        'content': 'data(label)',
        'text-valign': 'center',
        'text-halign': 'center'
      }
    },
    {
      selector: '$node > node',
      css: {
        //'content': 'data(label)',
        'padding-top': '10px',
        'padding-left': '10px',
    'background-color': 'green',
        'padding-bottom': '10px',
        'padding-right': '10px',
    'text-valign': 'top',
        'text-halign': 'center',
    'shape': 'roundrectangle'
      }
    },
    {
      selector: 'edge',
      css: {
        'content': 'data(label)',
        'width': 1,
        'line-color': 'black',
        "target-arrow-shape" : "triangle"
      }
    },
    {
      selector: ':selected',
      css: {
        'background-color': 'black',
        'line-color': 'black',
        'target-arrow-color': 'black',
        'source-arrow-color': 'black'
      }
    }
  ],

       showOverlay: false,
       minZoom: 0.1,
       maxZoom: 4.0,
       layout: {
         //name: 'preset',
         name: 'cose',
         fit: true
         },


    ready: function() {
        console.log("cy ready");
        cy = this;
        cy.load( cy.elements('*').jsons() );

        // readTextFile("file://io_edges");

        // 
        //jQuery.get('io_edges',function(data){
          //var file = data;

          //var res = s.split(" ");
          //alert(file.text());
        //});

        // buzz input

    document
    .getElementById('filechoice')
    .addEventListener(
        'change',
        function () {
            var fr = new FileReader();
            fr.onload = function () {
                document.getElementById('contents').textContent = this.result;
                
                //alert(this.result);
                
                var lines = this.result.split("\n"); 
                var traffictag = 0;
                var enforcetag = 0;
                var customizetag = 0;

                for (var i = 0; i<lines.length; i++) {
                  var line = lines[i];
                  if (line == "#Traffic")
                  {
                    traffictag = 1;
                    continue;
                  }
                  if (line == "#Enforcement")
                  {
                    enforcetag = 1;
                    continue;
                  }
                  if (line == "#Customize")
                  {
                    customizetag = 1;
                    continue;
                  }

                  if ((enforcetag == 1)&&(customizetag == 0))
                  {
                      var elems = line.split(" ");
                      addNodeWithFSM(elems[0]);
                      addNodeWithFSM(elems[2]);  
                      addEdgeWithID(elems[0],elems[2],elems[0]+elems[2],elems[1]);
                  }

                  if ((enforcetag == 1)&&(customizetag == 1))
                  {
                     var elems = line.split(" ");
                     modLabel(elems[0],elems[1]);
                     //alert(elems[0]);
                     //alert(elems[1]);

                  }

                  //addNodeWithID(elems[0]);
                  //addNodeWithID(elems[2]);  
                  //addEdgeWithID(elems[0],elems[2],elems[0]+elems[2],elems[1]);                
                }
                freshLayout();
                //alert(res[1]);
                //
                //addNodeWithID("FW1");
                //addNodeWithID("IDS1");
                //addEdgeWithID("FW1","IDS1","FW1IDS1","Cond1");                

            };
            fr.readAsText(this.files[0]);
            //alert(fr.readAsText(this.files[0]));
            //alert(this.files[0].text());
        }
    );

        //addNodeWithID("FW1");
        //addNodeWithID("IDS1");
        //addEdgeWithID("FW1","IDS1","FW1IDS1","Cond1");
        //freshLayout();

        $("#nested").click(function(){
          cy.$('node > node').css({visibility:'visible'});
          //alert("input");          
        });
        $("#click").click(function(){
          cy.$('node > node').css({visibility:'hidden'});
          //alert("input");          
        });

        cy.on('tap', 'node', function(e) {
            var evtTarget = event.cyTarget;
            //cy.$('node > node').css({visibility:'hidden',});
            }); // cy.on

/*        
        cy.on('tap', function(e) {
            if(e.cyTarget === cy) {
               var idNum = cy.nodes().size();
               setID = idNum.toString();
               offset = $cy.offset();
                position = {
                   x: e.originalEvent.pageX - offset.left,
                   y: e.originalEvent.pageY - offset.top
                   };
                cy.add([{group: "nodes",
                          data: {"id": "n" + setID,
                                 "resources": [],
                                 "properties": []
                                 },
                        renderedPosition: {x: position.x,
                                           y: position.y
                                           },
                        }]); // cy.add
                } // cyTarget === cy
            }); // cy.on

*/

        } // cy ready
        

   }); // cy initializer: cytoscape
})  // document ready
