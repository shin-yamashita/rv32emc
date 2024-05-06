
// rv_spi.sv
//  spi interface
//
// 2024/04/17

`timescale 1ns/1ns
`include "rv_types.svh"

module rv_spi (
    input  logic clk,
    input  logic xreset,
// bus
    input  u5_t  adr,
    input  logic cs,
    input  logic rdy,
    input  u4_t  we,
    input  logic re,
    input  u32_t dw,
    output u32_t dr,
// port
    input  logic spiclk,    // spi base clock 200M
    output logic sck,
    input  logic sdi,
    output logic sdo,
    output logic scs
    );

    function automatic u4_t to_gray(u4_t bin);
        return bin ^ (bin >> 1);
    endfunction

    // Registers
    u8_t  tfifo[16], rfifo[16];

    u8_t brc, rbrc, n_brc, r_brc;
    logic tcbr, tcbri, rxct, txen, txen_d, txen_s, resetp, resetp_s, rxcap;

    u4_t trp, trp_d, twp;
    u4_t twp_gr, twp_gr_s;
    logic tx_empty;
    u8_t  tx, rx;
    u3_t  tbc, rbc; // tx rx bit count

    typedef enum logic [1:0] {Idle, StartBit, Trans} state_t;
    state_t tst;

    always_ff @(posedge clk) begin
        txen_s <= txen;
        if(!xreset) begin
            dr <= '0;
        end else if(rdy) begin
                if(cs)
                    case(adr[4:2])
                    0:  dr <= {n_brc, r_brc, {6'd0,txen_s, scs}, 8'(twp)};
                    1:  dr <= {rfifo[3], rfifo[2], rfifo[1], rfifo[0]};
                    2:  dr <= {rfifo[7], rfifo[6], rfifo[5], rfifo[4]};
                    3:  dr <= {rfifo[11], rfifo[10], rfifo[9], rfifo[8]};
                    4:  dr <= {rfifo[15], rfifo[14], rfifo[13], rfifo[12]};
                    default:
                        dr <= '0;
                    endcase
                else
                    dr <= '0;
        end

        if(!xreset) begin
            twp  <= 'd0;
            twp_gr <= 'd0;
            scs  <= 1'b1;
            resetp <= 1'b0;
            n_brc <= 'd0;
            r_brc <= 'd0;
        end else begin
            if(cs && (adr[4:2]=='0) && rdy) begin
                if(we[0]) begin
                    tfifo[twp] <= dw[7:0];
                    twp <= twp + 'd1;
                    twp_gr <= to_gray(twp + 'd1);
                end
                if(we[1]) begin
                    resetp <= dw[9];
                    scs <= dw[8];
                end
                if(we[2]) begin
                    r_brc <= dw[23:16];
                end
                if(we[3]) begin
                    n_brc <= dw[31:24];
                end
            end else begin
                if(resetp) begin
                    twp <= 'd0;
                    twp_gr <= 'd0;
                end
            end
        end
    end

    // clock gen
    always_ff @(posedge spiclk) begin   // 200M 400M
        if(!xreset) begin
            brc <= 8'd0;
            rbrc <= 8'd0;
        end else begin
            if(brc < n_brc) begin    // count n_brc-1
                brc <= brc + 8'd1;
            end else begin
                brc <= 8'd0;
            end
            if(brc == (n_brc >> 1)+8'd1) begin
                rbrc <= 8'd0;
            end else if(rbrc < n_brc) begin    // count n_brc-1
                rbrc <= rbrc + 8'd1;
            end else begin
                rbrc <= 8'd0;
            end
        end
    end

    assign tx_empty = twp_gr_s == to_gray(trp);
    assign sdo = tx[7];

    always_ff @(posedge spiclk) begin // tx
        twp_gr_s <= twp_gr; // async
        resetp_s <= resetp;

        if(!xreset) begin
            sck   <= 1'b0;
            trp   <= 'd0;
            tst   <= Idle;
            tbc   <= 'd0;
            rbc   <= 'd0;
            txen  <= 1'b0;
            txen_d <= 1'b0;
            tx    <= 'd0;
            trp_d <= 'd0;
            tcbr  <= 1'b0;
            tcbri <= 1'b0;
            rxct  <= 1'b0;
            rxcap <= 1'B0;
        end else begin
            tcbr  <= brc == '0;
            tcbri <= brc > (n_brc>>1);
            rxct  <= rbrc == r_brc;
            //if(rbrc == r_brc && !(n_brc == r_brc && tbc == 'd0)) txen_d <= txen;
            if(rbrc == r_brc) txen_d <= txen;

            if(!txen || tcbr) begin 
                sck <= 1'b0;
            end else if(tcbri) begin
                sck <= 1'b1;
            end 

            if(resetp_s) begin
                trp <= 'd0;
            end else if(tcbr) begin

                case (tst)
                    Idle: begin
                        if(!tx_empty) begin  // tfifo not empty
                            tx  <= tfifo[trp];
                            trp <= trp + 'd1;
                            tst <= Trans;
                            txen <= 1'b1;
                        end else begin
                            txen <= 1'b0;
                        end
                        tbc  <= 3'd0;
                        rbc  <= 3'd0;
                    end
                    Trans: begin
                        tx  <= {tx[6:0], 1'b0};
                        tbc <= tbc + 'd1;
                        if(tbc == 'd7) begin
                            trp_d <= trp;
                            if(!tx_empty) begin  // tfifo not empty
                                tx  <= tfifo[trp];
                                trp <= trp + 'd1;
                                tst <= Trans;
                            end else begin
                                txen <= 1'b0;
                                tst <= Idle;
                            end
                        end
                    end
                    default:
                        tst <= Idle;
                endcase
            end
            if(rxct) begin
                if(txen_d)
                    rbc <= rbc + 'd1;
                rxcap <= (rbc == 3'd7) && txen_d;
                rx <= {rx[6:0], sdi};
                if(rxcap) begin
                    rfifo[trp_d-4'd1] <= rx;
                end
            end
        end
    end


endmodule
