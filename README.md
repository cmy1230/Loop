Currently, the project can accept TRAM's Verilog or ADG as input

You can get TRAM and how to use TRAM from https://github.com/yhqiu16/TRAM

Verilog file is CGRA.v in TRAM-main\cgra-mg

ADG file is cgra_adg.json in TRAM-main\cgra-mg\src\main\resources

If you want to run Chisel for TRAM with combinational logic loops, you can refer to the following commands:

  sbt "runMain mg.CGRAMG -td ./test_run_dir" --no-check-comb-loops
  
Moreover, if you have not added “--no-check-comb-loops” and successfully run chisel, it indicates that the circuit does not contain combinational logic loops

After placing the input in the corresponding position, run the C++code to obtain the output

Output is in the breakloop.xdc file

Place the output in TRAM-main\cgra-mg\src\main\scala\dsa\template\CGRA.scala, and regenerate Verilog and ADG to obtain TRAM CGRA without a combinational logic loop
