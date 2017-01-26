	;
	; 1 Zeichen in den Anzeigenspeicher schreiben (8 von 8)
	; Übergabe : Zeichencode in Char, Position in Col, 
	; Font Startadresse in AFont (16Bit), Start Längentabelle in AFl

SetPDig	movlb	1
	call	FPntr		; 1. Byte laden
	tablrd	0,1,Zeichen6	; Daten in Zwischenspeicher kopieren 
	tablrd	0,1,Zeichen5
	tablrd	0,1,Zeichen4
	tablrd	0,1,Zeichen3
	tablrd	0,1,Zeichen2
	tablrd	0,1,Zeichen1
	tablrd	0,1,Zeichen

	movlw	5
	movwf	Len		; Länge = 5
	movlw	Zeichen
	movwf	FSR0		; FSR0 auf erstes Bitmuster

	movfp	Col,WREG
	movwf	Col1		; Startkolonne zwischenspeichern

	clrf	ZCntr		; 1. Zeile

SPD0	movfp	Len,WREG
	movwf	CCntr		; Zähler auf Startwert
	call	SetZl		; FSR1 vorbereiten
	movfp	Col1,WREG
	movwf	Col		; Kolonne auf Startwert

SPD1	movfp	ZBase,FSR1	; Startwert für FSR1 (Zeilenstart)
	movfp	Col,WREG
	movwf	RCol		; echte Kolonne aus Tabelle laden
	andlw	B'11111000'	; Bits maskieren	-> Byte bestimmen
	rrncf	WREG		; /2
	rrncf	WREG		; /4
	rrncf	WREG		; /8 -> 8 Bit pro Byte !
	addwf	FSR1,F		; zu FSR addieren
	movfp	RCol,WREG	; nochmal laden
	andlw	B'00000111'	; Bitnummer maskieren	-> Bit bestimmen
	movwf	ZBit		; und speichern	

	btfsc	INDF0,4		; Bit des Zeichenmusters testen
	goto	SPD2

	call	ClrDot		; Punkt löschen
	goto	SPD3

SPD2	call	SetDot		; Punkt setzen

SPD3	rlncf	INDF0		; Bitmuster verschieben
	incf	Col		; nächste Kolonne
	decfsz	CCntr
	goto	SPD1		; nächster Punkt (5x)

	incf	ZCntr		
	incf	FSR0		; nächstes Bitmuster
	movlw	7
	cpfseq	ZCntr		; nächste Zeile (7x)
	goto	SPD0
	call	ClrCol
	return

ClrCol	movlw	7		; Zeilenzähler auf Startwert
	movwf	ZCntr

CC1	decf	ZCntr
	call	SetZl		; FSR1 vorbereiten
	incf	ZCntr
	movfp	Col,WREG
	movwf	RCol		; echte Kolonne aus Tabelle laden
	andlw	B'11111000'	; Bits maskieren	-> Byte bestimmen
	rrncf	WREG		; /2
	rrncf	WREG		; /4
	rrncf	WREG		; /8 -> 8 Bit pro Byte !
	addwf	FSR1,F		; zu FSR addieren
	movfp	RCol,WREG	; nochmal laden
	andlw	B'00000111'	; Bitnummer maskieren	-> Bit bestimmen
	movwf	ZBit		; und speichern	

	call	ClrDot		; Punkt löschen

	decfsz	ZCntr		; nächste Zeile (7x)
	goto	CC1

	incf	Col		; Nächste Kolonne
	return

	;
	; Bit im Speicher setzen
	;
SetDot	clrf	WREG		; löschen
	incf	ZBit		; erhöhen, um einen Durchgang zu erzwingen
	bsf	_Carry		; Carry setzen
SetDot1	rlcf	WREG		; Maske verschieben
	decfsz	ZBit		; nächstes Bit
	goto	SetDot1		; Neuer Durchgang
	movlr	1
	iorwf	INDF1,F		; Bit setzen
	movlr	0
	return

	;
	; Bit im Speicher löschen
	;
ClrDot	movlw	0xff		; setzen
	incf	ZBit		; erhöhen, um einen Durchgang zu erzwingen
	bcf	_Carry		; Carry löschen
ClrDot1	rlcf	WREG		; Maske verschieben
	decfsz	ZBit		; nächstes Bit
	goto	ClrDot1		; Neuer Durchgang
	movlr	1
	andwf	INDF1,F		; Bit löschen
	movlr	0
	return

	;
	; Setzt die Adresse der in ZCntr angegebenen Zeile in FSR1
	;
SetZl	movlw	D'20'
	mulwf	ZCntr		; ZCntr x 20
	movfp	PRODL,WREG	; Ergebnis laden
	addlw	Z1		; +Basisadresse Zeile 1
	movwf	FSR1		; ins FSR1
	movwf	ZBase		; und in Zwischenspeicher
	return

	;
	; 1 Zeichen in den Anzeigenspeicher schreiben (5 von 8)
	;
SetDig	call	FPntr		; Zeiger in Fontspeicher bilden

	movlw	Z7_1		; Startadresse Zeile 1
	addwf	Digit,W		; Offset addieren
	movwf	FSR1
	tablrd	0,1,INDF1	; Daten in Anzeigenspeicher kopieren 

	movlw	Z6_1		; Startadresse Zeile 2
	addwf	Digit,W		; Offset addieren
	movwf	FSR1
	tablrd	0,1,INDF1	; Daten in Anzeigenspeicher kopieren 

	movlw	Z5_1		; Startadresse Zeile 3
	addwf	Digit,W		; Offset addieren
	movwf	FSR1
	tablrd	0,1,INDF1	; Daten in Anzeigenspeicher kopieren 

	movlw	Z4_1		; Startadresse Zeile 4
	addwf	Digit,W		; Offset addieren
	movwf	FSR1
	tablrd	0,1,INDF1	; Daten in Anzeigenspeicher kopieren 

	movlw	Z3_1		; Startadresse Zeile 5
	addwf	Digit,W		; Offset addieren
	movwf	FSR1
	tablrd	0,1,INDF1	; Daten in Anzeigenspeicher kopieren 

	movlw	Z2_1		; Startadresse Zeile 6
	addwf	Digit,W		; Offset addieren
	movwf	FSR1
	tablrd	0,1,INDF1	; Daten in Anzeigenspeicher kopieren 

	movlw	Z1_1		; Startadresse Zeile 7
	addwf	Digit,W		; Offset addieren
	movwf	FSR1
	tablrd	0,1,INDF1	; Daten in Anzeigenspeicher kopieren 

	return

	; Zeiger in Zeichensatz bilden, 1.Byte laden
FPntr	movlw	0x20
	subwf	Char,F		; Zeichencode konvertieren
	movlw	0x10
	cpfseq	Char
	goto	FPntr1
	movlw	D'047'
	movwf	Char		; Konvertierung 0->O

FPntr1	movfp	Char,WREG
	movwf	TBLPTRL		; Tablepointer laden
	clrf	TBLPTRH		; Highbyte löschen
	bcf	_Carry		; Carry löschen
	rlcf	TBLPTRL
	rlcf	TBLPTRH		; x2
	rlcf	TBLPTRL
	rlcf	TBLPTRH		; x4
	rlcf	TBLPTRL
	rlcf	TBLPTRH		; x8
	movlw	low Font
	addwf	TBLPTRL,F	; Startadresse addieren Low
	movlw	high Font
	addwfc	TBLPTRH,F	; Startadresse addieren High + Carry
	tablrd	0,1,WREG	; Dummy-read -> TBL=1.Bitmuster des Zeichens

	return

	; Interrupt Routine, Taktet eine Zeile der Anzeige
LEDIRQ	movlb	1		; Bank 1 aktivieren
	clrwdt

	movlw	Z1_1		; Start Zeichenspeicher
	addwf	Zeile,W		; Zeile addieren
	addwf	Zeile,W
	addwf	Zeile,W		; x3
	decf	WREG		; Zeiger auf Ende setzen		
	movwf	FSR0

	movpf	INDF0,SerOut	; Daten kopieren
	call	ShByte		; und ausgeben
	decf	FSR0
	movpf	INDF0,SerOut	; Daten kopieren
	call	ShByte		; und ausgeben
	decf	FSR0
	movpf	INDF0,SerOut	; Daten kopieren
	call	ShByte		; und ausgeben

	movfp	Zeile,WREG
	rlncf	WREG		; x2
	rlncf	WREG		; x4
	addwf	Zeile,W		; x5
	rlncf	WREG		; x10
	rlncf	WREG		; x20
	addlw	Z1		; Basisadresse addieren,
	decf	WREG		; -1 für echten Anfang
	movwf	DPtr		; als Zeiger setzen
	
	movlw	_AnzByte
	movwf	ICount		; Bytezähler setzen

IRQ1	movfp	DPtr,FSR0	; Zeiger setzen
	movlr	1
	movpf	INDF0,WREG	; Bitmuster holen
	movlr	0
	movwf	SerOut		; und in Ausgabespeicher

	call	ShByte		; Bitmuster ausgeben

	decf	DPtr		; nächstes Byte

	decfsz	ICount		; nächstes Byte ausgeben
	goto	IRQ1

	movpf	ZTreib,SerOut	; Muster für Zeilentreiber laden
	call	ShByte		; und ausgeben

	bcf	_EN		; Anzeige aus

	movlw	0x08
IRQ2	nop			; Pause bei Zeilenumschaltung zur Schattenvermeidung
	decfsz	WREG
	goto	IRQ2

	bsf	_Strobe
	nop
	bcf	_Strobe		; Strobe erzeugen

	bsf	_EN		; Anzeige ein

	incf	Zeile		; nächste Zeile
	rrncf	ZTreib		; nächstes Muster für Zeilentreiber	

	movlb	1
	bcf	PIR,6		; IRQ Flag löschen
	bcf	_Data

	movlw	8
	cpfseq	Zeile		; Zeile 8 erreicht ( Überlauf !)
	return

	movlw	1
	movwf	Zeile		; Zeile wieder 0
	movlw	_ZTreibM
	movwf	ZTreib		; Muster für Zeilentreiber auf Startwert

	; Hier folgt die Tastenabfrage !
	movlb	0
	movfp	_Tasten,WREG	; Tastenzustand laden
	xorlw	B'00111111'	; und Invertieren
	andlw	B'00111111'	; unerwünschte Bits maskieren
	iorwf	Taster,F	; zu vorhandenen Informationen hinzufügen

	return

;	ShiftByte, schreibt ein Byte in die Registerkette (Dynamisch)

ShByte	movlw	0x8		; Zähler laden
SB1	rlcf	SerOut
	bsf	_Data		; Data = 1
	btfss	_Carry		; Carry testen
	bcf	_Data		; Data = 0

	bsf	_Clock
	bcf	_Clock		; Clock erzeugen

	decfsz	WREG
	goto	SB1

	return

