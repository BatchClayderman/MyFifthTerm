`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 04/25/2021 08:19:41 PM
// Design Name: 
// Module Name: add_x4
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


module add_x4(
    input signed [4:0] A,//被加减数
    input signed [4:0] X,//加减数
    input sel,//选择加减法（0是加法，1是减法）
    output signed [4:0] C,
    output reg V//溢出标记
    );
    wire [4:0] tmp = (sel ? ~X[4:0] + 1'b1 : X);
    assign C = (sel ? A - X : A + X);//数值还没有处理成补码
    always@(A or X)
    begin
    if (A[4] == tmp[4] && A[4] == C[4])
        V = 0;//左起第一位相加的进位值和左起第二位相加的进位值进行比较,相同则不溢出
    else
        V = 1;
    end  
endmodule