// To handle errors in a consistant way from ajax
//
function is_error(data) {
  if ( data.error == 1 ) 
    return true;
  else
    return false;
}

// Fling up a dialog window
//
function reg_process_error(data) {
  err_dialog.setBody( data.t1 );
  err_dialog.cfg.setProperty("icon",YAHOO.widget.SimpleDialog.ICON_WARN);
  err_dialog.show();
}

// Handle the "recents" feature.  Keep the most recent 9 register
// visits in order of most recent to least recent.  Delete least
// recent when adding a 10th item.  NOTE THE constant value of '6'
// which is the index to insert new items right after the <hr> at
// the bottom of the menu!
//
function add_recent(r) {
  if ( recents_array.length > 8 ) {
    // throw away the least recently used
    var item = recents_array.pop();
    mainMenu.removeItem( item );
    item.destroy();
  }
  var item = new YAHOO.widget.MenuItem
    ( r.name, { text: r.name, target: r.id } );
  recents_array.unshift( item );
  // fix up the main menu...
  mainMenu.insertItem( item, 6 ); // <---- beware of the 6!
}

// Reset the recents array
//
function reset_recents() {
  var len = recents_array.length;
  for(var i=0; i<len; i++) {
    var item = recents_array.pop();
    mainMenu.removeItem( item );
    item.destroy();
  }
}

// Main page layoput
//
function reg_layout() {
  var layout = new YAHOO.widget.Layout({
    units: [
      {
        position: 'right',
	body:	  'maps',
	header:	  'Map Hierarchy',
	width:    200,
	resize:	  true,
	gutter:	  '5px',
	collapse: true,
	scroll:	  true,
	animate:  true
      },
      {
        position: 'left',
	body:	  'registers',
	header:	  'Registers',
	width:    200,
	resize:	  true,
	gutter:	  '5px',
	collapse: true,
	scroll:	  true,
	animate:  true
      },
      {
        position: 'top',
	height:   40,
	resize:	  false,
	gutter:	  '0px',
	body:	  'nav'
      },
      {
        position: 'center',
	body:	  'register',
	gutter:	  '5px 0 0',
	scroll:	  true
      }
    ]
  });
  layout.render();
  return layout;
}

function add_bottom_panel( rid ) {
  // Gene on 9/11/12:  changed header to include server and port
  var my_header = 'Dynamic R/W on ' + context.server + ":" + context.port;
  var my_dyn_panel = mainLayout.addUnit({ position: 'bottom',
                   gutter:  '5px',
	           collapse: true,
	           scroll:   true,
	           animate:  true,
		   height:   75,
		   body:     'dyn_panel',
		   header:   my_header
  });
  // fill it with the form
  var res = $.ajax({
    type: 'POST',
    url:  context.baseurl + 'dynamic/fill_panel',
    async: false
  }).responseText;
  $("#dyn_panel").html( res );
  if ( rid ) {
    dyn_panel_addresses(rid);
  }
  else {
    dyn_panel_addresses(context.lastr_id);
  }
}

function dyn_panel_addresses(rid) {
  if ( context.dyn && rid ) {
    context.id1 = rid;
    context.nocache = new Date().getTime();
    $.getJSON( context.baseurl + 'dynamic/reg_addresses',
             context,
	     function(data) {
	       if ( is_error( data ) ) {
	         reg_process_error( data );
	       }
	       else {
	         $("#dyn_panel_address").find("option").remove();
		 $("#dyn_panel_value").val('');
		 for( var i=0; i<data.addresses.length; i++ ) {
		   var addr = data.addresses[i];
		   var str  = '<option value="' + addr + '">' + addr + '</option>';
		   $("#dyn_panel_address").append( str ); 
		 }
		 $("#dyn_panel_instance_wrapper").html('');
		 if ( data.instances ) {
		   $("#dyn_panel_instance_wrapper").append('<b>Instance:</b>');
		   $("#dyn_panel_instance_wrapper").append("<select id=\"dyn_panel_instance\">" );
		   for ( var i=0; i<data.instances.length; i++ ) {
		     var inst = data.instances[i];
		     $("#dyn_panel_instance").append('<option value="' + inst + '">' + inst + '</option>');
		   }
		 }
	       }
	     }
    );
  }
}

function dyn_panel_update_address(addr) {
  $("#dyn_panel_address").find("option:selected").removeAttr('selected');
  $("#dyn_panel_address").find("option").each(function() {
    if ( $(this).val() == addr ) {
      $(this).attr('selected', 'selected');
    }
  });
}

function cancel_dyn_mode() {
  if ( context.dyn  ) {
    context.dyn = 0;
    var p = mainLayout.getUnitByPosition( 'bottom' );
    if ( p ) {
      p.close();
    }
  }
}

// Set the database text prompt just before showing it to the user
//
function db_dialog_set_db() {
  $("#dbDialog").find("#database").val( context.database );
}

// Main menu handler
//
function reg_menu_handler(type,args) {
  var event = args[0];
  var menuItem = args[1];
  if ( menuItem ) {
    var text = menuItem.cfg.getProperty('text');
    if ( menuItem.index == 0 ) {
      // new database
      db_dialog_set_db();
      db_dialog.show();
    }
    else if ( menuItem.index == 1 ) {
      // pdf
      reg_do_pdf();
    }
    else if ( menuItem.index == 2 ) {
      // connect
      scan_dialog.show();
    }
    else if ( menuItem.index == 3 ) {
      // ihier
      reg_do_ihier();
    }
    else if ( menuItem.index == 4 ) {
      cust_dialog.show();
    }
    else if ( menuItem.index >= 6 ) {
      // Its a "recent".  The reg id is in...
      // NOTE THE constant value of 6!  This is the index of
      // the next item after the <hr> in the menu!
      //
      var rid = menuItem.cfg.getProperty('target');
      reg_load_registers(rid);
      reg_load_register(rid);
    }
  }
}

// Create the main menu
//
function reg_menu() {
  var mainMenu = new YAHOO.widget.Menu("mainmenu");
  mainMenu.addItems([
    { text: "New database..." },
    { text: "PDF Book..." },
    { text: "Connect..." },
    { text: "Interrupts..." },
    { text: "Change Customer..." },
    { text: "<hr />", disabled: true }
  ]);
  mainMenu.render(document.body);
  mainMenu.subscribe( 'click', reg_menu_handler );
  return mainMenu;
}

// Create static dialogs ...
//
function reg_db_dialog() {
  var dlog = new YAHOO.widget.Dialog("dbDialog",
    { fixedcenter: true, visible: false, postmethod: "manual"
    });
  dlog.cfg.queueProperty('buttons', [
    { text: "Load", 
      isDefault: true,
      handler: function() {
        db_dialog_submit();
      } } ] );
  dlog.render();
  return dlog;
}

function db_dialog_submit() {
  context.database = $("#dbDialog").find("#database").val();
  db_dialog.hide();
  reg_load_register();
  reg_load_registers();
  reg_load_maps();
  cancel_dyn_mode();
}

function reg_as_dialog() {
  var dlog = new YAHOO.widget.Dialog("advSearch",
    { fixedcenter: true, visible: false
    });
  dlog.cfg.queueProperty('buttons', [
    { text: "Search", isDefault: true,
      handler: function() {
        reg_show_loading( center_loading );
        $.ajax({
	  type: 'POST',
	  url: context.baseurl + 'search/adv_search',
	  data: $("#advSearch").find("form").serialize() + '&database=' + context.database + '&customer=' + context.customer,
	  dataType: 'json',
	  success: function(json) {
	    center_loading.hide();
	    if ( is_error( json ) ) {
	      reg_process_error( json );
	    }
	    else {
	      $("#register").html(json.t1);
	    }
	  }
	});
      } },
    { text: "Done", isDefault: false,
      handler: function() {
	this.hide();
      } }
      ] );
  dlog.render();
  return dlog;
}

function reg_cust_dialog() {
  var d = new YAHOO.widget.Dialog("custDialog",
    { fixedcenter: true, visible: false
    });
  d.cfg.queueProperty('buttons', [
    { text: "Load", isDefault: true,
      handler: function() {
        context.customer = this.getData().customer;
	this.hide();
	reg_load_register(context.lastr_id);
	reg_load_registers(context.lastr_id);
	reg_load_maps(context.lastr_id);
      } } ] );
  d.render();
  return d;
}

function reg_err_dialog() {
  var mySimpleDialog = new YAHOO.widget.SimpleDialog("dlg", { 
	width: "40em", 
	effect:{effect:YAHOO.widget.ContainerEffect.FADE,
	        duration:0.25}, 
	fixedcenter:true,
	modal:true,
        visible:false,
	draggable:false });
  mySimpleDialog.setHeader("Warning!");
  mySimpleDialog.setBody( 'This is your last warning!' );
  mySimpleDialog.cfg.setProperty("icon",YAHOO.widget.SimpleDialog.ICON_WARN);
  mySimpleDialog.render(document.body); 
  return mySimpleDialog;
}

function reg_scan_dialog() {
  var d = new YAHOO.widget.Dialog("scanDialog",
    { fixedcenter: true, visible: false
    });
  d.cfg.queueProperty('buttons', [
    { text: "Scan!", isDefault: true,
      handler: function() {
        context.server   = this.getData().server;
        context.port     = this.getData().port;
        context.protocol = this.getData().protocol;
	context.ld_library_path = this.getData().ldpath;
	context.t3       = this.getData().dbname;
	this.hide();
	do_scan('scan');
      } }, 
    { text: "Show All",
      handler: function() {
        context.server   = this.getData().server;
        context.port     = this.getData().port;
        context.protocol = this.getData().protocol;
	context.ld_library_path = this.getData().ldpath;
	context.t3       = this.getData().dbname;
	this.hide();
	do_scan('show');
      } }, 
    { text: "Cancel",
      handler: function() {
	this.hide();
      } }
    ] );
  d.render();
  return d;
}

function reg_loading_dialog(ref) {
  var reference = document.getElementById( ref );
  var ldc = new YAHOO.widget.Panel(ref+"wait",
    { close: false,
      draggable: false,
      zindex: 4,
      visible: false,
      width: '240px',
      fixedcenter: true
    }
  );
  ldc.render();
  return ldc; 
}

function reg_show_loading(w) {
  w.show();
}

function reg_int_dialog() {
  var d = new YAHOO.widget.SimpleDialog("interruptDialog", {
		   width: "45em", 
		   fixedcenter:true,
		   modal:false,
		   visible:false,
		   draggable:true });

		 d.cfg.queueProperty("buttons",
		   [ { text: "Close",
		       handler: function() {
		         this.hide();
		       },
		       isDefault: true
		     }
		   ]
		 );
		 d.render(document.body);
  return d;
}

function reg_mod_dialog() {
  var d = new YAHOO.widget.SimpleDialog("moduleDialog", {
		   width: "45em", 
		   fixedcenter:true,
		   modal:false,
		   visible:false,
		   draggable:true });

		 d.cfg.queueProperty("buttons",
		   [ { text: "Close",
		       handler: function() {
		         this.hide();
		       },
		       isDefault: true
		     }
		   ]
		 );
		 d.render(document.body);
  return d;
}

function reg_bulk_dialog() {
  var d = new YAHOO.widget.SimpleDialog("bulkDialog", {
		   width: "45em", 
		   fixedcenter:true,
		   modal:false,
		   visible:false,
		   draggable:true });

		 d.cfg.queueProperty("buttons",
		   [ { text: "Close",
		       handler: function() {
		         this.hide();
		       },
		       isDefault: true
		     }
		   ]
		 );
		 d.render(document.body);
  return d;
}

/////////////////////////////////////////////////////////
// MAIN INITIALIZATION FUNCTION
// Called once the page is loaded.  
////////////////////////////////////////////////////////
function reg_initialize() {
  mainLayout     = reg_layout();
  mainMenu       = reg_menu();

  db_dialog    = reg_db_dialog();
  cust_dialog  = reg_cust_dialog();
  err_dialog   = reg_err_dialog();
  as_dialog    = reg_as_dialog();
  scan_dialog  = reg_scan_dialog();
  int_dialog   = reg_int_dialog();
  mod_dialog   = reg_mod_dialog();
  bulk_dialog   = reg_bulk_dialog();

  left_loading   = reg_loading_dialog('registers');
  right_loading  = reg_loading_dialog('maps');
  center_loading = reg_loading_dialog('register');

  $("#dialogs").css( 'display', 'block' );

  if ( context.database == 'undefined' ) {
    db_dialog.show();
  }
  else {
    reg_load_register();
    reg_load_registers();
    reg_load_maps();
  }
}

// Load the left pane via ajax
//
function reg_load_registers(id) {
  context.id1 = id || 0;
  context.nocache = new Date().getTime();
  reg_show_loading( left_loading );
  $.getJSON( context.baseurl + 'reg/load_registers',
             context,
	     function(data) {
	       left_loading.hide();
	       if ( is_error( data ) ) {
	         reg_process_error( data );
	       }
	       else {
	         $("#registers").html(data.t1);
		 $("#nav").find("#bottom span").html(data.t2);
	       }
	     }
  );
}

// Load the right pane via ajax
//
function reg_load_maps(id) {
  context.id1 = id || 0;
  context.nocache = new Date().getTime();
  reg_show_loading( right_loading );
  $.getJSON( context.baseurl + 'reg/load_maps',
             context,
	     function(data) {
	       right_loading.hide();
	       if ( is_error( data ) ) {
	         reg_process_error( data );
	       }
	       else {
	         $("#maps").html(data.t1);
		 // Convert the map hierarchy to a tree
		 var tree = new YAHOO.widget.TreeView("maptree");
		 var tree_root = tree.getRoot();
		 tree.render();
		 var top = tree_root.children[0];
		 // expand the topmost level
		 top.expand();
		 // display the map title in the nav pane
		 $("#nav").find("#bottom span").html(data.t2);
	       }
	     }
  );
}

// Load the center window with register data via ajax
//
function reg_load_register(id) {
  context.id1 = id || 0;
  context.nocache = new Date().getTime();
  reg_show_loading( center_loading );
  $.getJSON( context.baseurl + 'reg/load_register',
             context,
	     function(data) {
	       center_loading.hide();
	       if ( is_error( data ) ) {
	         reg_process_error( data );
	       }
	       else {
	         $("#register").html(data.t1);
		 // remember this as the last register seen
		 context.lastr_id = data.lastr_id;
		 context.lastr_name = data.lastr_name;
		 // add to the recents function on the main menu
		 add_recent({id: context.lastr_id,
		             name: context.lastr_name});
	         dyn_panel_addresses(id);
		 dyn_reg_encodings();
	       }
	     }
  );
}

// User selected a register from list of registers
//
function reg_select(id) {
  reg_load_register(id);
  $("#registers").find(".selected").removeClass('selected');
  $("#reg"+id).addClass('selected');
}

// User makes a map selection
//
function reg_map_select(rid,maptitle) {
  reg_load_registers(rid);
  reg_load_register(rid);
  // display map title in nav pane
  $("#nav").find("#bottom span").html(maptitle);
}

// User selectes a register from search results, or
// any place outside of the registers panel
//
function register_select(id) {
  reg_load_registers(id);
  reg_load_register(id);
  reg_load_maps(id);
}  

function advanced_search() {
  as_dialog.show();
}

function reg_do_pdf() {
  reg_show_loading( center_loading );
  var data = $("#pdfPrompt").find("form").serialize();
  if ( ! data ) {
    data = 'database=' + context.database + '&customer=' + context.customer;
  }
  $.ajax({
    type: 'POST',
    url: context.baseurl + 'pdf/prompt',
    data: data,
    dataType: 'json',
    success: function(json) {
      center_loading.hide();
      if ( is_error( json ) ) {
        reg_process_error( json );
      }
      else {
        $("#register").html(json.t1);
      }
    }
  });
  return false;
}

function reg_render_pdf() {
  $("#loading").html("Rendering... Please wait <img src=\"/static/images/loading.gif\" />");
  $("#pdf_example").find('form').submit();
  return false;
}

function reg_do_ihier() {
  context.nocache = new Date().getTime();
  reg_show_loading( center_loading );
  if ( context.dyn ) {
    $.getJSON( context.baseurl + 'dynamic/iview',
               context,
	       function(data) {
	         center_loading.hide();
	         if ( is_error( data ) ) {
	           reg_process_error( data );
	         }
	         else {
	           $("#register").html(data.t1);
	         }
	       }
    );
  }
  else {
    $.getJSON( context.baseurl + 'ihier/start',
               context,
	       function(data) {
	         center_loading.hide();
	         if ( is_error( data ) ) {
	           reg_process_error( data );
	         }
	         else {
	           $("#register").html(data.t1);
		   $(".ihier").treeview();
	         }
	       }
    );
  }
}

function ifill(cname, inext, index, base) {
  $("#next").val(inext);
  $("#base").val(base);
  $("#index").val(index);
  reg_show_loading( center_loading );
  var res = $.ajax({
    type: 'POST',
    url: context.baseurl + 'ihier/ifill',
    data: $("#params").serialize(),
    dataType: 'json',
    success: function(json) {
      center_loading.hide();
      if ( is_error( json ) ) {
        reg_process_error( json );
      }
      else {
        var branch = $("#"+cname).append(json.t1);
	var tree = $(".ihier");
	tree.treeview({
	  add: branch
	});
      }
    }
  });
}

function do_scan(type) {
  context.t1 = type;
  context.nocache = new Date().getTime();
  reg_show_loading( center_loading );
  $.getJSON( context.baseurl + 'dynamic/scan',
             context,
	     function(data) {
	       center_loading.hide();
	       if ( is_error( data ) ) {
	         reg_process_error( data );
	       }
	       else if ( data.error == 2 ) {
	         // refresh register view
		 cancel_dyn_mode();

		 context.dyn = 1;
		 reg_load_register(context.lastr_id);
		 reg_load_registers(context.lastr_id);
		 reg_load_maps(context.lastr_id);

		 add_bottom_panel();
	       }
	       else {
	         $("#register").html(data.t1);
	       }
	     }
  );
}

function reg_submit_show_all() {
  context.database = $("#show_all_form").find(":checked").val();
  reset_recents();
  reg_load_register();
  reg_load_registers();
  reg_load_maps();
  cancel_dyn_mode();
  return false;
}

/////// Register Access Routines ////////////////////////////

// Show the value of a register read on the screen.
// Also used to display a write ...
//
function reg_read_show_value( data ) {
  $("#dyn_panel_value").val( data.value );
  for( var i=0; i<data.bits.length; i++ ) {
    $("#rbit"+i).find('span').html(data.bits[i] || '0' );
  }

  var ii;
  if ( data.fields ) {
    for( ii in data.fields ) {
      var bf = data.fields[ii];
      $("#bfield"+bf.id).html( bf.value );
    }    
  }

  dyn_reg_encodings();

  if ( data.custom_bulk ) {
    $("#bulkDialog").find("#bulk-results-container").html( data.custom_bulk );
    bulk_dialog.show();
  }

  custom_output( data );
}

// Show any custom register output data
//
function custom_output( data ) {
  if ( data.custom_output ) {
    for( var i=0; i<data.custom_output.length; i++ ) {
      var field = data.custom_output[i];
      $("#custom-input-form").find("#"+field.name).val( field.value );
    }
  }
}

// User clicked on a nit in the bittable.  Toggle the bit
// (and its affect on value) in preparation for a write.
function reg_toggle_bit(i) {
  var cur = $("#rbit"+i).find('span').html();
  var nxt = '1';
  if ( cur == '1' ) {
    nxt = '0';
  }
  $("#rbit"+i).find('span').html(nxt);

  // Send current register value (if any), bit index and regid
  // to server to get back a new value.
  context.id1 = context.lastr_id;
  context.id2 = i;
  context.t1  = $("#dyn_panel_value").val();
  context.t2  = nxt;
  context.nocache = new Date().getTime();
  $.getJSON( context.baseurl + 'dynamic/reg_bit_toggle',
             context,
	     function(data) {
	       center_loading.hide();
	       if ( is_error( data ) ) {
	         reg_process_error( data );
	       }
	       else {
	         $("#dyn_panel_value").val( data.value );
		 dyn_reg_encodings();
	       }
	     }
  );
}

function reg_read(addr,inst,custom) {
  // addr is a hex formatted string
  context.id1 = context.lastr_id;
  context.id2 = inst;
  context.t1  = addr;
  context.t3  = custom;
  context.nocache = new Date().getTime();
  reg_show_loading( center_loading );
  $.getJSON( context.baseurl + 'dynamic/reg_read',
             context,
	     function(data) {
	       center_loading.hide();
	       if ( is_error( data ) ) {
	         reg_process_error( data );
	       }
	       else {
	         reg_read_show_value( data );
	       }
	     }
  );
}

function reg_write(addr,value,inst,custom) {
  // addr is a hex formatted string
  context.id1 = context.lastr_id;
  context.id2 = inst;
  context.t1  = addr;
  context.t2  = value;
  context.t3  = custom;
  context.nocache = new Date().getTime();
  reg_show_loading( center_loading );
  $.getJSON( context.baseurl + 'dynamic/reg_write',
             context,
	     function(data) {
	       center_loading.hide();
	       if ( is_error( data ) ) {
	         reg_process_error( data );
	       }
	       else {
	         reg_read_show_value( data );
	       }
	     }
  );
}

function dyn_panel_module_fn( fn ) {
  var addr = $("#dyn_panel_address").val();
  context.id1 = context.lastr_id;
  context.t1  = fn;
  context.t2  = addr;
  reg_show_loading( center_loading );
  $.getJSON( context.baseurl + 'dynamic/module_fn',
             context,
	     function(data) {
	       center_loading.hide();
	       if ( is_error( data ) ) {
	         reg_process_error( data );
	       }
	       else {
	         $("#moduleDialog").find("#mod-results-container").html( data.t1 );
		 mod_dialog.show();
	       }
	     }
  );
}

function dyn_panel_read_reg() {
  var addr = $("#dyn_panel_address").val();
  var inst = $("#dyn_panel_instance").val();
  var custom = $("#custom-input-form").serialize();
  reg_read( addr, inst, custom );  
}

function dyn_panel_write_reg() {
  var addr  = $("#dyn_panel_address").val();
  var inst = $("#dyn_panel_instance").val();
  var value = $("#dyn_panel_value").val();
  var custom = $("#custom-input-form").serialize();
  reg_write( addr, value, inst, custom );
}

function dyn_reg_encodings() {
  if ( context.dyn ) {
    context.t1 = $("#dyn_panel_value").val();
    $(".bitfield-encodings").each(function() {
      var tid = $(this).attr('tid');
      var bid = $(this).attr('bid');
      context.id1 = bid;
      context.id2 = tid;
      context.nocache = new Date().getTime();
      $.getJSON( context.baseurl + 'dynamic/reg_fetch_bitfield_encodings',
             context,
	     function(data) {
	       if ( ! is_error( data ) ) {
	         $("#dyn_enc_"+bid).html( data.t1 );
	       }
	     }
      );
    });
  }
}

function dyn_enc_change(id) {
  var val = $("#"+id).find("option:selected").val();
  var bid = $("#"+id).attr('bid');
  context.id1 = bid;
  context.id2 = val;
  context.t1 = $("#dyn_panel_value").val();
  context.nocache = new Date().getTime();
  $.getJSON( context.baseurl + 'dynamic/bf_enc_change',
             context,
	     function(data) {
	       if ( is_error( data ) ) {
	         reg_process_error( data );
	       }
	       else {
	         reg_read_show_value( data );
	       }
	     }
  );  
}

function dyn_addr_select(addr) {
  var custom = $("#custom-input-form").serialize();
  dyn_panel_update_address(addr);
  reg_read( addr, custom );
}

function app_toggle_display(id) {
  $(id).each(function() {
    if ( $(this).css( 'display' ) == 'none' ) {
      $(this).css( 'display', 'block' );
    }
    else {
      $(this).css( 'display', 'none' );
    }
  });
}

function app_cvt_toggle( id ) {
  var str = $("#"+id).html();
  var ival = parseInt( str );
  var rval;
  if ( str.search( /^0x/ ) ) {
    rval = '0x' + ival.toString(16);
  }
  else {
    rval = ival.toString(10);
  }
  $("#"+id).html(rval);
}
