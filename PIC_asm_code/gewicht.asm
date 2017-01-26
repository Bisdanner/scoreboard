
	include c:\pic\common\regs.def

	include Gew_Mem.ASM

	constant _AnzByte=D'20'		; Anzahl der Datenbytes in der Reisterkette
	constant _ZTreibM=B'01000000'	; Startmuster für Zeilentreiber
	constant _Leer=0

	org	0x0000		; Programmstartvektor

 	goto	Start

	org	0x0020		; IRQ Vektor für Timer3

IRQ	movpf	WREG,ISave	; WREG retten
	movpf	BSR,ISave1	; Bank select laden
	movpf	FSR0,ISave2	; FSR0 retten
	movpf	ALUSTA,ISave3	; ALUSTA retten

	clrwdt

	movlb	1		; Bank 1 einschalten
	movlr	0		; RAM-Bank 0

	btfsc	PIR,6		; IRQ durch Timer 3 ?
	call	LEDIRQ		; Anzeige beschreiben

	movlb	1
	btfsc	PIR,0		; IRQ durch RX ?
	call	SerIRQ		; Byte empfangen

	movfp	ISave3,ALUSTA	; ALUSTA wiederherstellen
	movfp	ISave2,FSR0	; FSR wiederherstellen
	movfp	ISave1,BSR	; BSR wiederherstellen
	movfp	ISave,WREG	; WREG wiederherstellen
	retfie

	org	0x40

	; IRQs vorbereiten und Timer 3 einstellen

Start	bsf	CPUSTA,4	; alle IRQs abschalten
	movlb	1		; Bank 1 ein
	bsf	PIE,6		; Timer 3 IRQ Enable setzen
	bsf	PIE,0		; RX IRQ Enable setzen
	bsf	INTSTA,3	; Peripheral IRQs einschalten
	movlb	2		; Bank 2 aktivieren
	movlw	0x0f
	movfp	WREG,PR3H
	movlw	0xa0		; Period-register für Timer 3 auf 0x1f40 setzen
	movfp	WREG,PR3L	; entspricht 4.000 = 1ms bei 16 MHz
	movlb	3
	bcf	TCON2,3		; Timer 3 in Period Mode
	bsf	TCON2,2		; Timer 3 aktivieren

	movlb	0		; Bank 0 aktivieren
	setf	PORTA
	movlw	B'00100000'
	movpf	WREG,TXSTA	; Schnittstelle TX einstellen
	movlw	B'10010000'	
	movpf	WREG,RCSTA	; Receiver einstellen
	movlw	D'25'
	movpf	WREG,SPBRG	; 25 = 9600 Baud bei 16 MHz
	clrf	PORTB		; PORTB löschen
	setf	DDRB		; PORTB = Input (6 Taster)

	movlb	1
	clrf	PORTD
	clrf	PORTC
	clrf	DDRD		; Port D -> Output (Registerkette)
	clrf	DDRC		; Port C -> Output (Lampen + Hupe)
	clrf	DDRE		; Port E -> Output
	clrf	Zeile		; Startzeile = 0

	movlw	0x1a
	movwf	FSR0
Clear0	clrf	INDF0		; RAM löschen (0)
	movlr	1
	clrf	INDF0		; RAM löschen (1)
	movlr	0
	incfsz	FSR0
	goto	Clear0

	movlw	_ZTreibM
	movwf	ZTreib		; Startmuster für Zeilentreiber
	incf	RXS		; RXS auf 1

	movlb	1
	bcf	CPUSTA,4	; IRQs einschalten
	bsf	_EN

Loop	call	DisTest
	;goto	Loop

Main	movlb	0
	btfsc	RCSTA,1		; Overrun Error ?
	goto	Main1

	btfss	RXFlag,0	; Daten empfangen ?
	goto	Main

	call	SetAnz
	goto	Main

Main1	bcf	RCSTA,4
	bsf	RCSTA,4		; OERR löschen
	movlw	1
	movwf	RXS		; Empfangsstatus auf 1
	clrf	RXFlag
	goto	Main

	; Decodieren der empfangenen Daten und setzen des Flags zum einschreiben
SetAnz	movlw	SerData+D'28'		; Adresse Datenspeicher
	movwf	Work2			; in Zwischenspeicher
	movlw	D'0'			; Startdigit 0
	movwf	Digit
	movlw	low Font
	movwf	AFont
	movlw	high Font
	movwf	AFont1

	movfp	Work2,FSR1		; Zeiger vorbereiten  *** 3 Digits setzen ***
	movpf	INDF1,Char		; Daten kopieren
	call	SetDig
	incf	Work2			; nächstes Zeichen
	incf	Digit
	movfp	Work2,FSR1		; Zeiger vorbereiten
	movpf	INDF1,Char		; Daten kopieren
	call	SetDig
	incf	Work2			; nächstes Zeichen
	incf	Digit
	movfp	Work2,FSR1		; Zeiger vorbereiten
	movpf	INDF1,Char		; Daten kopieren
	call	SetDig
	incf	Work2			; nächstes Zeichen

	;btfsc	RXFlag,1		; Kurzer Datensatz ?
	;goto	Peri2

SALoop	movlw	0			
	movwf	Work3			; Zeilennummer
	movlw	SerData
	movwf	Work4			; Startoffset

	call	WrZeile			; Zeile beschreiben

	movlw	1			
	movwf	Work3			; Zeilennummer
	movlw	SerData+D'14'
	movwf	Work4			; Startoffset

	call	WrZeile			; Zeile beschreiben

	clrf	RXFlag			; Empfangsflag löschen

	; Schalten der Lampen und der Hupe(n)
Peri	movlb	1
	clrf	Work2		; Zwischenspeicher löschen
	movlw	A'0'		; '0' = Inaktiv
	cpfseq	SerData+D'31'	; 1. Lampe
	bsf	Work2,0		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'32'	; 2. Lampe
	bsf	Work2,1		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'33'	; 3. Lampe
	bsf	Work2,2		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'34'	; 4. Lampe
	bsf	Work2,3		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'35'	; 5. Lampe
	bsf	Work2,4		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'36'	; 6. Lampe
	bsf	Work2,5		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'37'	; 1. Hupe
	bsf	Work2,6		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'38'	; 2. Hupe (extern)
	bsf	Work2,7		; Wenn aktiv, Bit setzen
	movfp	Work2,Lampen	; Informationen setzen
	movfp	SerData+D'39',WREG	; Taster-Reset Byte
	movwf	Work2
	movlw	A'1'
	cpfseq	Work2		; Reset für Taster-Information ?
	goto	Answer		; Nein, Ende

	clrf	Taster		; Ja, Speicher löschen
	goto	Answer

	; Schalten der Lampen und der Hupe(n) bei kurzem Datensatz
Peri2	movlb	1
	clrf	Work2		; Zwischenspeicher löschen
	movlw	A'0'		; '0' = Inaktiv
	cpfseq	SerData+D'31'	; 1. Lampe
	bsf	Work2,0		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'32'	; 2. Lampe
	bsf	Work2,1		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'33'	; 3. Lampe
	bsf	Work2,2		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'34'	; 4. Lampe
	bsf	Work2,3		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'35'	; 5. Lampe
	bsf	Work2,4		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'36'	; 6. Lampe
	bsf	Work2,5		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'37'	; 1. Hupe
	bsf	Work2,6		; Wenn aktiv, Bit setzen
	cpfseq	SerData+D'38'	; 2. Hupe (extern)
	bsf	Work2,7		; Wenn aktiv, Bit setzen
	movfp	Work2,Lampen	; Informationen setzen
	movfp	SerData+D'39',WREG	; Taster-Reset Byte
	movwf	Work2
	movlw	A'1'
	cpfseq	Work2		; Reset für Taster-Information ?
	goto	Answer		; Nein, Ende

	clrf	Taster		; Ja, Speicher löschen

Answer	movlw	D'2'		; Antwort senden
	call	TXData
	movlw	'0'
	btfsc	Taster,0	; Taster 1 gedrückt ?
	movlw	'1'
	call	TXData
	movlw	'0'
	btfsc	Taster,1	; Taster 2 gedrückt ?
	movlw	'1'
	call	TXData
	movlw	'0'
	btfsc	Taster,2	; Taster 3 gedrückt ?
	movlw	'1'
	call	TXData
	movlw	'0'
	btfsc	Taster,3	; Taster 4 gedrückt ?
	movlw	'1'
	call	TXData
	movlw	'0'
	btfsc	Taster,4	; Taster 5 gedrückt ?
	movlw	'1'
	call	TXData
	movlw	'0'
	btfsc	Taster,5	; Taster 6 gedrückt ?
	movlw	'1'
	call	TXData
	movlw	D'3'
	call	TXData

	bcf	Flag,0			; Flag löschen
	return

	; Beschreibt eine der Anzeigezeilen
	; Zeile in Work3, Start im Empfangspuffer in Work4

WrZeile	movlw	D'80'		; 80
	mulwf	Work3		; xZeile -> Startkolonne
	movpf	PRODL,Col	; Kolonne setzen

	addwf	Col,W		; Kolonne + 80 in WREG
	movwf	Work1		; als Letzte Kolonne setzen

	movlw	D'13'
	movwf	Digit		; Zeichenzähler

WZ1	movfp	Work4,FSR1	; Zeiger vorbereiten
	movpf	INDF1,Char	; Daten kopieren
	call	SetPDig

	incf	Work4		; nächstes Zeichen
	decfsz	Digit
	goto	WZ1

	return

WZE	movfp	Work1,WREG	; letzte Kolonne erreicht ?
	cpfslt	Col
	return
	call	ClrCol		; Kolonne löschen
	goto	WZE

	; Interrupt Routine, Empfängt Daten über die Serielle Schnittstelle
SerIRQ	movlb	1
	bcf	_EN		; Anzeige aus
	movlb	0

	movlw	1		; Status prüfen
	cpfsgt	RXS
	goto	Status1		; Empfange StartByte 0x02
	movlw	2
	cpfsgt	RXS
	goto	Status2 	; Empfange Daten + EndByte 0x03
	movlw	3
	cpfsgt	RXS
	goto	Status3 	; Empfange Prüfsumme

SerErr	movlw	1
	movwf	RXS		; Status wieder 1
	clrf	Check		; Prüfsumme zurücksetzen
	goto	SerEnde

Status1	movlw	D'2'		; Startzeichen
	cpfseq	RCREG		
	goto	SerErr		; Nein, Fehler

	incf	RXS		; Status = 2
	clrf	ZZ		; Zeichenzähler löschen
	movlw	2
	movwf	Check		; Prüfsumme bilden
	goto	SerEnde

Status2	movlw	D'3'		; Ende empfangen ? (ETX)
	cpfseq	RCREG
	goto	ST21

	addwf	Check,F		; zu Prüfsumme addieren
	incf	RXS		; RXS = 3
	goto	SerEnde

ST21	movlw	SerData		; Adresse Datenspeicher
	addwf	ZZ,W		; Zähler addieren
	movwf	FSR0		; Als Zeiger speichern
	movfp	RCREG,WREG
	movwf	INDF0		; Zeichen speichern
	addwf	Check,F		; Zeichen zu Checksumme addieren
	incf	ZZ		; Zeichenzähler ehöhen
	movlw	D'55'
	cpfseq	ZZ		; schon 55 Zeichen empfangen ? (zu lang!)
	goto	SerEnde
	nop
	goto	SerErr		; Empfang abbrechen

Status3	movfp	Check,WREG
	cpfseq	RCREG		; Prüfsumme korrekt ?
	goto	SerErr		; Nein, Datensatz verwerfen

	bsf	RXFlag,1
	movlw	D'12'
	cpfseq	ZZ		; Datensatz mit 12 Zeichen ?
	bcf	RXFlag,1	; Nein, alles einschreiben

	bsf	RXFlag,0	; Korrekt, DatenFlag setzen
	goto	SerErr		; Empfang beenden

SerEnde	movlb	1
	bsf	_EN		; Anzeige ein
	bcf	PIR,0		; IRQ Flag löschen
	return

	; Ein Byte ausgeben (in WREG)
TXData	movlb	0
TXD1	btfss	TXSTA,1		; TSR Leer ?
	goto	TXD1		; Nein, Warten
	movwf	TXREG		; Byte ausgeben
	return

	; Schaltet alle Anzeigen einmal ein und wieder aus
DisTest	movlw	3
	movwf	Work2		; Zähler = 3

DT0	call	DT1		; Anzeigen setzen

	call	Pause		; 125ms Pause

	call	DT2		; Anzeigen löschen

	call	Pause

	decfsz	Work2
	goto	DT0
	return

DT1	movlw	Z1_1
	movwf	FSR1

DT10	setf	INDF1
	incf	FSR1
	movlw	Z7_3+1
	cpfseq	FSR1
	goto	DT10

	movlw	Z1		; Start Zeilenspeicher
	movwf	FSR1
	movlr	1		; RAM-Bank 1

	movlw	0xff		; Anzeigemuster
DT11	movwf	INDF1
	incfsz	FSR1		; Speicher bis zum Ende füllen
	goto	DT11

	movlr	0		; RAM-Bank 0
	movlw	B'00111111'
	movwf	Lampen		; Alle Lampen an
	return

DT2	movlw	Z1_1
	movwf	FSR1

DT20	clrf	INDF1
	incf	FSR1
	movlw	Z7_3+1
	cpfseq	FSR1
	goto	DT20

	movlw	Z1		; Start Zeilenspeicher
	movwf	FSR1
	movlr	1		; RAM-Bank 1

	movlw	0		; Anzeigemuster
DT22	movwf	INDF1
	incfsz	FSR1		; Speicher bis zum Ende füllen
	goto	DT22

	movlr	0		; RAM-Bank 0
	clrf	Lampen		; Lampen aus
	return

	; Pausenschleife ca. 0,2 bzw 0,5 Sekunden
PauseK	movlw	D'100'		; x0,5 = 50ms
	goto	Pause1
Pause	movlw	D'250'		; x0,5 = 125ms
Pause1	call	Pause2
	decfsz	WREG
	goto	Pause1
	return

Pause2	nop			; 0,5ms bei 16 MHz
	nop
	nop
	nop
	nop
	decfsz	PTC		; Zähler -1
	goto	Pause2
	return

	include Gew_IRQ.asm

	include c:\pic\common\Font_sm.asm

	End
