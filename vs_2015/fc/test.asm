;16 bit unsigned multiply

      *=$8000
      LDA #$FF
      STA $20
      LDA #$11
      STA $21
      LDA #$FF
      STA $22
      LDA #$11
      STA $23


      LDA #$00
      STA $24
      LDA #$00
      STA $25
      LDA #$00
      STA $26
      LDA #$00
      STA $27

loop  LDA $22
      BNE go
      LDA $23
      BNE go
      JMP term
go    CLC
      LDA $24
      ADC $20
      STA $24
      LDA $25
      ADC $21
      STA $25

      LDA $26
      ADC #$00
      STA $26
      LDA $27
      ADC #$00
      STA $27

      DEC $22
      LDA $22
      CMP #$FF
      BEQ equal
      JMP loop
equal DEC $23
      JMP loop

term  BRK