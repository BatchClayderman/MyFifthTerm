`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/25/2021 08:37:49 PM
// Design Name: 
// Module Name: sim_1
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module sim_1(

    );
    reg [4:0] A = 5'b01100;
    reg [4:0] X = 5'b01000;
    wire sel = 0;//选择加减法（0是加法，1是减法）
    wire [4:0] C;
    always
        #5 A = A + 1;
    always
        #7 X = X + 1;
    add_x4 add_x4(A, X, sel, C, V);
endmodule
