; File y:\tech\parinfo\cur_ver\msdos\parinfo.cpp
; Line 37
; btHardDrive = 0006
; pDiskParam = 0008
_GetDriveParam:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02315
	call	FAR PTR __aFchkstk
	push	si
	push	di
; wSeg = fffc
; bResult = fffa
; wOff = fff8
; Line 38
	mov	WORD PTR -6[bp],OFFSET 0
; Line 39
; Line 40
	push	OFFSET 26
	push	OFFSET 0
	push	WORD PTR 10[bp]
	push	WORD PTR 8[bp]
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 41
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:[bx],OFFSET 26
; Line 42
	lea	bx,WORD PTR 8[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR -8[bp],ax
; Line 43
	lea	bx,WORD PTR 8[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:2[bx]
	mov	WORD PTR -4[bp],ax
; Line 44
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _IsSupportExt13
	add	sp,OFFSET 2
	mov	WORD PTR ?blSupportExt13@@3HE,ax
; Line 45
	cmp	WORD PTR ?blSupportExt13@@3HE,OFFSET 0
	je	L01616
; Line 46
; Line 47
; Line 48
; Line 49
	push	ds
; Line 50
	mov	ah,OFFSET 72
; Line 51
	mov	dl,BYTE PTR 6[bp]
; Line 52
	mov	si,WORD PTR -8[bp]
; Line 53
	mov	ds,WORD PTR -4[bp]
; Line 54
	int	OFFSET 19
; Line 55
	pop	ds
; Line 56
	jb	NOT_SUPPORT13EXT
; Line 57
	mov	bx,WORD PTR -8[bp]
; Line 58
	test	WORD PTR 2[bx],OFFSET 2
; Line 59
	je	NOT_SUPPORT13EXT
; Line 60
	mov	WORD PTR -6[bp],OFFSET 1
; Line 61
NOT_SUPPORT13EXT:
; Line 62
; Line 63
; Line 64
L01616:
	cmp	WORD PTR -6[bp],OFFSET 0
	jne	L01618
; Line 65
; Line 66
; Line 67
; Line 68
	mov	ah,OFFSET 8
; Line 69
	mov	dl,BYTE PTR 6[bp]
; Line 70
	int	OFFSET 19
; Line 71
	jb	END
; Line 72
	mov	WORD PTR -6[bp],OFFSET 1
; Line 73
	mov	bx,WORD PTR -8[bp]
; Line 74
	mov	BYTE PTR 8[bx],dh
; Line 75
	inc	BYTE PTR 8[bx]
; Line 76
	mov	dx,cx
; Line 77
	and	dx,OFFSET -64
; Line 78
	ror	dx,OFFSET 8
; Line 79
	shr	dh,OFFSET 6
; Line 80
	inc	dx
; Line 81
	and	cx,OFFSET 63
; Line 82
	mov	WORD PTR 12[bx],cx
; Line 83
	mov	WORD PTR 4[bx],dx
; Line 84
END:
; Line 85
; Line 86
; Line 87
L01618:
	les	bx,WORD PTR 8[bp]
	cmp	WORD PTR es:8[bx],OFFSET 0
	jne	L01620
	cmp	WORD PTR es:10[bx],OFFSET 0
	jne	L01620
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:8[bx],OFFSET 255
	mov	WORD PTR es:10[bx],OFFSET 0
; Line 88
L01620:
	les	bx,WORD PTR 8[bp]
	cmp	WORD PTR es:12[bx],OFFSET 0
	jne	L01621
	cmp	WORD PTR es:14[bx],OFFSET 0
	jne	L01621
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:12[bx],OFFSET 63
	mov	WORD PTR es:14[bx],OFFSET 0
; Line 89
L01621:
	les	bx,WORD PTR 8[bp]
	cmp	WORD PTR es:4[bx],OFFSET 0
	jne	L01622
	cmp	WORD PTR es:6[bx],OFFSET 0
	jne	L01622
; Line 90
	les	bx,WORD PTR 8[bp]
	push	WORD PTR es:14[bx]
	push	WORD PTR es:12[bx]
	les	bx,WORD PTR 8[bp]
	push	WORD PTR es:10[bx]
	push	WORD PTR es:8[bx]
	les	bx,WORD PTR 8[bp]
	push	WORD PTR es:18[bx]
	push	WORD PTR es:16[bx]
	call	FAR PTR __aFuldiv
	push	dx
	push	ax
	call	FAR PTR __aFuldiv
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:4[bx],ax
	mov	WORD PTR es:6[bx],dx
; Line 91
L01622:
	les	bx,WORD PTR 8[bp]
	cmp	WORD PTR es:16[bx],OFFSET 0
	jne	L01623
	cmp	WORD PTR es:18[bx],OFFSET 0
	jne	L01623
; Line 92
	les	bx,WORD PTR 8[bp]
	push	WORD PTR es:14[bx]
	push	WORD PTR es:12[bx]
	les	bx,WORD PTR 8[bp]
	push	WORD PTR es:10[bx]
	push	WORD PTR es:8[bx]
	les	bx,WORD PTR 8[bp]
	push	WORD PTR es:6[bx]
	push	WORD PTR es:4[bx]
	call	FAR PTR __aFulmul
	push	dx
	push	ax
	call	FAR PTR __aFulmul
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:16[bx],ax
	mov	WORD PTR es:18[bx],dx
; Line 93
L01623:
	les	bx,WORD PTR 8[bp]
	cmp	WORD PTR es:24[bx],OFFSET 0
	jne	L01624
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:24[bx],OFFSET 512
; Line 94
L01624:
	mov	ax,WORD PTR -6[bp]
	jmp	L01608
; Line 95
; Line 95
L01608:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 98
; btHardDrive = 0006
_IsSupportExt13:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02319
	call	FAR PTR __aFchkstk
	push	si
	push	di
; btResult = fffc
; wTemp = fffa
; Line 99
	mov	WORD PTR -6[bp],OFFSET 1
; Line 100
	mov	BYTE PTR -4[bp],OFFSET 0
; Line 101
; Line 102
; Line 103
	mov	ax,OFFSET 16640
; Line 104
	mov	bx,OFFSET 21930
; Line 105
	mov	dl,BYTE PTR 6[bp]
; Line 106
	stc
; Line 107
	int	OFFSET 19
; Line 108
	jb	NOT_SUPPORT
; Line 109
	sub	bx,OFFSET -21931
; Line 110
	mov	WORD PTR -6[bp],bx
; Line 111
NOT_SUPPORT:
; Line 112
; Line 113
	cmp	WORD PTR -6[bp],OFFSET 0
	jne	L02321
	mov	ax,OFFSET 1
	jmp	L02320
L02321:
	mov	ax,OFFSET 0
L02320:
	jmp	L01626
; Line 114
; Line 114
L01626:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 117
; dwExtStart = 0006
; pPartitionMBR = 000a
; pDriveParam = 000e
?IsValidEMBR@@ZAHKPEUPARTITION_SEC@@PEUBIOS_DRIVE_PARAM@@@Z:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02322
	call	FAR PTR __aFchkstk
	push	si
	push	di
; bResult = fffc
; dwMaxSectors = fff8
; PartiEntry = ffd8
; Line 118
; Line 119
	mov	WORD PTR -4[bp],OFFSET 0
; Line 120
; Line 121
	les	bx,WORD PTR 10[bp]
	cmp	WORD PTR es:510[bx],OFFSET -21931
	jne	L01638
; Line 122
; Line 123
	push	OFFSET 32
	push	OFFSET 0
	lea	ax,WORD PTR -40[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 124
	push	OFFSET 32
	lea	ax,WORD PTR -40[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	ax,OFFSET 478
	mov	cx,WORD PTR 10[bp]
	mov	dx,WORD PTR 12[bp]
	add	cx,ax
	push	dx
	push	cx
	call	FAR PTR _memcmp
	add	sp,OFFSET 10
	cmp	ax,OFFSET 0
	jne	L01641
; Line 125
; Line 126
	les	bx,WORD PTR 14[bp]
	mov	ax,WORD PTR es:16[bx]
	mov	dx,WORD PTR es:18[bx]
	mov	WORD PTR -8[bp],ax
	mov	WORD PTR -6[bp],dx
; Line 127
	push	OFFSET 16
	lea	ax,WORD PTR -40[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	ax,OFFSET 446
	mov	cx,WORD PTR 10[bp]
	mov	dx,WORD PTR 12[bp]
	add	cx,ax
	push	dx
	push	cx
	call	FAR PTR _memcmp
	add	sp,OFFSET 10
	cmp	ax,OFFSET 0
	je	L01643
; Line 128
; Line 129
	les	bx,WORD PTR 10[bp]
	mov	al,BYTE PTR es:450[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01644
; Line 130
; Line 131
; Line 132
	les	bx,WORD PTR 10[bp]
	mov	ax,WORD PTR -8[bp]
	mov	dx,WORD PTR -6[bp]
	cmp	WORD PTR es:456[bx],dx
	ja	L02324
	jb	L02325
	cmp	WORD PTR es:454[bx],ax
	jae	L02324
L02325:
	les	bx,WORD PTR 10[bp]
	mov	ax,WORD PTR -8[bp]
	mov	dx,WORD PTR -6[bp]
	cmp	WORD PTR es:460[bx],dx
	ja	L02324
	jb	L02326
	cmp	WORD PTR es:458[bx],ax
	jae	L02324
L02326:
	mov	ax,OFFSET 1
	jmp	L02323
L02324:
	mov	ax,OFFSET 0
L02323:
	mov	WORD PTR -4[bp],ax
; Line 134
; Line 135
L01644:
; Line 136
	jmp	L01645
L01643:
; Line 136
	mov	WORD PTR -4[bp],OFFSET 1
L01645:
; Line 137
	push	OFFSET 16
	lea	ax,WORD PTR -40[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	ax,OFFSET 462
	mov	cx,WORD PTR 10[bp]
	mov	dx,WORD PTR 12[bp]
	add	cx,ax
	push	dx
	push	cx
	call	FAR PTR _memcmp
	add	sp,OFFSET 10
	cmp	ax,OFFSET 0
	je	L01647
	les	bx,WORD PTR 10[bp]
	mov	al,BYTE PTR es:466[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01647
; Line 138
; Line 139
; Line 140
	les	bx,WORD PTR 10[bp]
	mov	ax,WORD PTR -8[bp]
	mov	dx,WORD PTR -6[bp]
	cmp	WORD PTR es:472[bx],dx
	ja	L01649
	jb	L02327
	cmp	WORD PTR es:470[bx],ax
	ja	L01649
L02327:
	les	bx,WORD PTR 10[bp]
	mov	ax,WORD PTR -8[bp]
	mov	dx,WORD PTR -6[bp]
	cmp	WORD PTR es:476[bx],dx
	jb	L01648
	ja	L02328
	cmp	WORD PTR es:474[bx],ax
	jbe	L01648
L02328:
L01649:
; Line 142
	mov	WORD PTR -4[bp],OFFSET 0
; Line 143
L01648:
; Line 144
L01647:
; Line 145
L01641:
; Line 146
L01638:
	mov	ax,WORD PTR -4[bp]
	jmp	L01634
; Line 147
; Line 147
L01634:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 150
; dwSectors = 0006
; btHardDisk = 000a
; btStartOrEnd = 000c
_LocatePartition:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02329
	call	FAR PTR __aFchkstk
	push	si
	push	di
; dwHeads = fffa
; dwHead1 = fff6
; DriveParam = ffdc
; dwSector = ffd8
; dwTrack = ffd4
; Line 151
; Line 152
	mov	WORD PTR -40[bp],OFFSET 0
	mov	WORD PTR -38[bp],OFFSET 0
; Line 153
	mov	WORD PTR -6[bp],OFFSET 0
	mov	WORD PTR -4[bp],OFFSET 0
; Line 154
; Line 156
	lea	ax,WORD PTR -36[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetDriveParam
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L01658
; Line 157
	mov	ax,OFFSET -1
	mov	dx,OFFSET -1
	jmp	L01653
; Line 159
L01658:
	cmp	WORD PTR 6[bp],OFFSET 63
	jne	L01659
	cmp	WORD PTR 8[bp],OFFSET 0
	jne	L01659
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	jne	L01659
; Line 160
	mov	ax,WORD PTR 6[bp]
	mov	dx,WORD PTR 8[bp]
	jmp	L01653
; Line 161
L01659:
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	jne	L01660
; Line 162
	mov	WORD PTR -40[bp],OFFSET 1
	mov	WORD PTR -38[bp],OFFSET 0
; Line 163
L01660:
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 1
	jne	L01661
; Line 164
; Line 165
	mov	ax,WORD PTR -24[bp]
	mov	dx,WORD PTR -22[bp]
	mov	WORD PTR -40[bp],ax
	mov	WORD PTR -38[bp],dx
; Line 166
	mov	ax,WORD PTR -28[bp]
	mov	dx,WORD PTR -26[bp]
	add	ax,OFFSET -1
	adc	dx,OFFSET -1
	mov	WORD PTR -6[bp],ax
	mov	WORD PTR -4[bp],dx
; Line 167
; Line 168
L01661:
	push	WORD PTR -26[bp]
	push	WORD PTR -28[bp]
	push	WORD PTR -22[bp]
	push	WORD PTR -24[bp]
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	call	FAR PTR __aFuldiv
	push	dx
	push	ax
	call	FAR PTR __aFuldiv
	mov	WORD PTR -44[bp],ax
	mov	WORD PTR -42[bp],dx
; Line 169
	push	WORD PTR -26[bp]
	push	WORD PTR -28[bp]
	push	WORD PTR -22[bp]
	push	WORD PTR -24[bp]
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	call	FAR PTR __aFuldiv
	push	dx
	push	ax
	call	FAR PTR __aFulrem
	mov	WORD PTR -10[bp],ax
	mov	WORD PTR -8[bp],dx
; Line 170
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	jne	L01663
; Line 171
; Line 172
	mov	ax,WORD PTR -28[bp]
	mov	dx,WORD PTR -26[bp]
	add	ax,OFFSET -1
	adc	dx,OFFSET -1
	cmp	WORD PTR -10[bp],ax
	jne	L01664
	cmp	WORD PTR -8[bp],dx
	jne	L01664
; Line 173
	add	WORD PTR -44[bp],OFFSET 1
	adc	WORD PTR -42[bp],OFFSET 0
; Line 174
L01664:
; Line 175
L01663:
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 1
	jne	L01665
; Line 176
; Line 177
	cmp	WORD PTR -10[bp],OFFSET 0
	jne	L01666
	cmp	WORD PTR -8[bp],OFFSET 0
	jne	L01666
; Line 178
	sub	WORD PTR -44[bp],OFFSET 1
	sbb	WORD PTR -42[bp],OFFSET 0
; Line 179
L01666:
; Line 180
L01665:
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	jne	L01667
; Line 181
; Line 182
	cmp	WORD PTR -10[bp],OFFSET 1
	jne	L01668
	cmp	WORD PTR -8[bp],OFFSET 0
	jne	L01668
; Line 183
	push	WORD PTR -22[bp]
	push	WORD PTR -24[bp]
	push	WORD PTR -26[bp]
	push	WORD PTR -28[bp]
	push	WORD PTR -42[bp]
	push	WORD PTR -44[bp]
	call	FAR PTR __aFulmul
	add	ax,WORD PTR -10[bp]
	adc	dx,WORD PTR -8[bp]
	push	dx
	push	ax
	call	FAR PTR __aFulmul
	add	ax,WORD PTR -40[bp]
	adc	dx,WORD PTR -38[bp]
	add	ax,OFFSET -1
	adc	dx,OFFSET -1
	jmp	L01653
; Line 184
	jmp	L01669
L01668:
; Line 184
; Line 185
	push	WORD PTR -22[bp]
	push	WORD PTR -24[bp]
	push	WORD PTR -26[bp]
	push	WORD PTR -28[bp]
	push	WORD PTR -42[bp]
	push	WORD PTR -44[bp]
	call	FAR PTR __aFulmul
	add	ax,WORD PTR -6[bp]
	adc	dx,WORD PTR -4[bp]
	push	dx
	push	ax
	call	FAR PTR __aFulmul
	add	ax,WORD PTR -40[bp]
	adc	dx,WORD PTR -38[bp]
	add	ax,OFFSET -1
	adc	dx,OFFSET -1
	jmp	L01653
L01669:
; Line 186
; Line 187
L01667:
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 1
	jne	L01670
; Line 188
	push	WORD PTR -22[bp]
	push	WORD PTR -24[bp]
	push	WORD PTR -26[bp]
	push	WORD PTR -28[bp]
	push	WORD PTR -42[bp]
	push	WORD PTR -44[bp]
	call	FAR PTR __aFulmul
	add	ax,WORD PTR -6[bp]
	adc	dx,WORD PTR -4[bp]
	push	dx
	push	ax
	call	FAR PTR __aFulmul
	add	ax,WORD PTR -40[bp]
	adc	dx,WORD PTR -38[bp]
	add	ax,OFFSET 0
	adc	dx,OFFSET 0
	jmp	L01653
; Line 189
L01670:
	mov	ax,OFFSET -1
	mov	dx,OFFSET -1
	jmp	L01653
; Line 190
; Line 190
L01653:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 193
; btHardDrive = 0006
; pParHard = 0008
_GetPartitionInfo:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02331
	call	FAR PTR __aFchkstk
	push	si
	push	di
; dwLogicStart = fffa
; i = fff8
; PartitionMBR = fdf8
; blResult = fdf6
; dwTempSec = fdf2
; DriveParam = fdd8
; Line 194
; Line 195
; Line 196
; Line 197
	mov	WORD PTR -522[bp],OFFSET 0
; Line 198
; Line 200
	lea	ax,WORD PTR -552[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetDriveParam
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L01680
; Line 201
	mov	ax,OFFSET 0
	jmp	L01673
; Line 203
L01680:
	lea	ax,WORD PTR -552[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -520[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	je	L01681
; Line 204
; Line 205
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 128
	jle	L01682
	mov	ax,WORD PTR -550[bp]
	and	ax,OFFSET 4
	cmp	ax,OFFSET 0
	je	L01682
; Line 206
; Line 207
	les	bx,WORD PTR 8[bp]
	mov	BYTE PTR es:8[bx],OFFSET 0
; Line 208
	mov	WORD PTR -8[bp],OFFSET 0
	jmp	L01686
L01683:
; Line 209
; Line 210
	mov	ax,WORD PTR -8[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -74[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01687
	mov	ax,WORD PTR -8[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -74[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 128
	je	L01687
; Line 211
; Line 212
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:[bx],OFFSET 1
; Line 213
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:12[bx],OFFSET 0
	mov	WORD PTR es:14[bx],OFFSET 0
; Line 214
	les	bx,WORD PTR 8[bp]
	mov	ax,WORD PTR -536[bp]
	mov	dx,WORD PTR -534[bp]
	mov	WORD PTR es:16[bx],ax
	mov	WORD PTR es:18[bx],dx
; Line 215
	mov	al,BYTE PTR -434[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 50
	jne	L01688
	mov	al,BYTE PTR -435[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 51
	jne	L01688
; Line 216
	les	bx,WORD PTR 8[bp]
	mov	BYTE PTR es:8[bx],OFFSET 11
; Line 217
L01688:
	mov	al,BYTE PTR -462[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 54
	jne	L01689
	mov	al,BYTE PTR -463[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 49
	jne	L01689
; Line 218
	les	bx,WORD PTR 8[bp]
	mov	BYTE PTR es:8[bx],OFFSET 6
; Line 219
L01689:
	mov	al,BYTE PTR -462[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 50
	jne	L01690
	mov	al,BYTE PTR -463[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 49
	jne	L01690
; Line 220
	les	bx,WORD PTR 8[bp]
	mov	BYTE PTR es:8[bx],OFFSET 4
; Line 221
L01690:
	les	bx,WORD PTR 8[bp]
	mov	al,BYTE PTR es:8[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	jne	L01691
; Line 222
; Line 223
	lea	ax,WORD PTR -552[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -520[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 6
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	je	L01692
; Line 224
; Line 225
	mov	al,BYTE PTR -434[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 50
	jne	L01693
	mov	al,BYTE PTR -435[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 51
	jne	L01693
; Line 226
	les	bx,WORD PTR 8[bp]
	mov	BYTE PTR es:8[bx],OFFSET 11
; Line 227
L01693:
	mov	al,BYTE PTR -462[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 54
	jne	L01694
	mov	al,BYTE PTR -463[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 49
	jne	L01694
; Line 228
	les	bx,WORD PTR 8[bp]
	mov	BYTE PTR es:8[bx],OFFSET 6
; Line 229
L01694:
	mov	al,BYTE PTR -462[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 50
	jne	L01695
	mov	al,BYTE PTR -463[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 49
	jne	L01695
; Line 230
	les	bx,WORD PTR 8[bp]
	mov	BYTE PTR es:8[bx],OFFSET 4
; Line 231
L01695:
; Line 232
L01692:
; Line 233
L01691:
	mov	ax,OFFSET 1
	jmp	L01673
; Line 234
; Line 235
L01687:
L01684:
; Line 235
	add	WORD PTR -8[bp],OFFSET 1
L01686:
	cmp	WORD PTR -8[bp],OFFSET 4
	jl	L01683
L01685:
; Line 236
; Line 237
L01682:
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:[bx],OFFSET 0
; Line 238
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:2[bx],OFFSET 0
; Line 240
	mov	WORD PTR -8[bp],OFFSET 0
	jmp	L01699
L01696:
; Line 241
; Line 242
	mov	ax,WORD PTR -8[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -74[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01700
; Line 243
; Line 244
	push	OFFSET 16
	mov	ax,WORD PTR -8[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	cx,WORD PTR -74[bp]
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	mov	ax,OFFSET 4
	mov	cx,WORD PTR 8[bp]
	mov	dx,WORD PTR 10[bp]
	add	cx,ax
	les	bx,WORD PTR 8[bp]
	mov	ax,WORD PTR es:[bx]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	add	cx,ax
	push	dx
	push	cx
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 245
	les	bx,WORD PTR 8[bp]
	add	WORD PTR es:[bx],OFFSET 1
; Line 246
; Line 247
L01700:
L01697:
; Line 247
	add	WORD PTR -8[bp],OFFSET 1
L01699:
	cmp	WORD PTR -8[bp],OFFSET 4
	jl	L01696
L01698:
; Line 248
	mov	WORD PTR -8[bp],OFFSET 0
	jmp	L01705
L01702:
; Line 249
; Line 250
; Line 251
	mov	ax,WORD PTR -8[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -74[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 5
	je	L01707
	mov	ax,WORD PTR -8[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -74[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 15
	jne	L01706
L01707:
; Line 252
; Line 253
	mov	ax,WORD PTR -8[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -74[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	WORD PTR -526[bp],ax
	mov	WORD PTR -524[bp],dx
; Line 254
	mov	ax,WORD PTR -526[bp]
	mov	dx,WORD PTR -524[bp]
	mov	WORD PTR -6[bp],ax
	mov	WORD PTR -4[bp],dx
; Line 255
	mov	WORD PTR -8[bp],OFFSET 0
	jmp	L01711
L01708:
; Line 256
; Line 257
	lea	ax,WORD PTR -552[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -520[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR -524[bp]
	push	WORD PTR -526[bp]
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	je	L01713
; Line 258
; Line 259
	lea	ax,WORD PTR -552[bp]
	mov	dx,ss
	push	dx
	push	ax
	lea	ax,WORD PTR -520[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	WORD PTR -4[bp]
	push	WORD PTR -6[bp]
	call	FAR PTR ?IsValidEMBR@@ZAHKPEUPARTITION_SEC@@PEUBIOS_DRIVE_PARAM@@@Z
	add	sp,OFFSET 12
	cmp	ax,OFFSET 0
	jne	L01714
	jmp	L01710
; Line 261
L01714:
	mov	al,BYTE PTR -70[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01715
; Line 262
; Line 263
	push	OFFSET 16
	lea	ax,WORD PTR -74[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	ax,OFFSET 68
	mov	cx,WORD PTR 8[bp]
	mov	dx,WORD PTR 10[bp]
	add	cx,ax
	mov	ax,OFFSET 20
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR -556[bp],cx
	mov	WORD PTR -554[bp],dx
	imul	WORD PTR es:2[bx]
	mov	cx,WORD PTR -556[bp]
	mov	dx,WORD PTR -554[bp]
	add	cx,ax
	mov	ax,OFFSET 4
	add	cx,ax
	push	dx
	push	cx
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 264
	mov	ax,OFFSET 68
	mov	bx,WORD PTR 8[bp]
	mov	cx,WORD PTR 10[bp]
	add	bx,ax
	mov	ax,OFFSET 20
	mov	WORD PTR -556[bp],bx
	mov	WORD PTR -554[bp],cx
	les	bx,WORD PTR 8[bp]
	imul	WORD PTR es:2[bx]
	mov	bx,WORD PTR -556[bp]
	mov	cx,WORD PTR -554[bp]
	add	bx,ax
	mov	es,cx
	mov	ax,WORD PTR -526[bp]
	mov	dx,WORD PTR -524[bp]
	mov	WORD PTR es:[bx],ax
	mov	WORD PTR es:2[bx],dx
; Line 265
	les	bx,WORD PTR 8[bp]
	add	WORD PTR es:2[bx],OFFSET 1
; Line 266
; Line 267
L01715:
	mov	al,BYTE PTR -54[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01717
; Line 268
	mov	ax,WORD PTR -6[bp]
	mov	dx,WORD PTR -4[bp]
	add	ax,WORD PTR -50[bp]
	adc	dx,WORD PTR -48[bp]
	mov	WORD PTR -526[bp],ax
	mov	WORD PTR -524[bp],dx
; Line 269
	jmp	L01718
L01717:
; Line 269
	jmp	L01710
L01718:
; Line 270
; Line 271
	jmp	L01719
L01713:
; Line 271
	jmp	L01710
L01719:
; Line 272
L01709:
; Line 272
	add	WORD PTR -8[bp],OFFSET 1
L01711:
	cmp	WORD PTR -8[bp],OFFSET 26
	jl	L01708
L01710:
; Line 273
	mov	ax,OFFSET 68
	mov	bx,WORD PTR 8[bp]
	mov	cx,WORD PTR 10[bp]
	add	bx,ax
	mov	ax,OFFSET 20
	mov	WORD PTR -556[bp],bx
	mov	WORD PTR -554[bp],cx
	les	bx,WORD PTR 8[bp]
	imul	WORD PTR es:2[bx]
	mov	bx,WORD PTR -556[bp]
	mov	cx,WORD PTR -554[bp]
	add	bx,ax
	mov	es,cx
	mov	WORD PTR es:[bx],OFFSET 0
	mov	WORD PTR es:2[bx],OFFSET 0
; Line 274
	jmp	L01704
; Line 275
; Line 276
L01706:
L01703:
; Line 276
	add	WORD PTR -8[bp],OFFSET 1
L01705:
	cmp	WORD PTR -8[bp],OFFSET 4
	jl	L01702
L01704:
; Line 277
	mov	WORD PTR -522[bp],OFFSET 1
; Line 278
; Line 279
L01681:
	mov	ax,WORD PTR -522[bp]
	jmp	L01673
; Line 280
; Line 280
L01673:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 283
; btHardDrive = 0006
; pParHard = 0008
_GetPartitionInfoEx:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02332
	call	FAR PTR __aFchkstk
	push	si
	push	di
; nCount = fffc
; OldParHard = fdb0
; i = fdae
; dwStartSec = fdaa
; Line 284
; Line 285
; Line 286
; Line 287
	mov	WORD PTR -594[bp],OFFSET 0
; Line 288
	push	OFFSET 588
	push	OFFSET 0
	lea	ax,WORD PTR -592[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 289
	lea	ax,WORD PTR -592[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetPartitionInfo
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L01728
; Line 290
	mov	ax,OFFSET 0
	jmp	L01722
; Line 291
L01728:
	les	bx,WORD PTR 8[bp]
	mov	ax,WORD PTR -590[bp]
	mov	WORD PTR es:2[bx],ax
; Line 292
	mov	ax,WORD PTR -592[bp]
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:[bx],ax
; Line 293
	push	OFFSET 64
	lea	ax,WORD PTR -588[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	ax,OFFSET 4
	mov	cx,WORD PTR 8[bp]
	mov	dx,WORD PTR 10[bp]
	add	cx,ax
	push	dx
	push	cx
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 294
	mov	WORD PTR -4[bp],OFFSET 0
	jmp	L01733
L01730:
; Line 295
; Line 296
; Line 297
	mov	ax,WORD PTR -4[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -588[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 5
	je	L01735
	mov	ax,WORD PTR -4[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -588[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 15
	jne	L01734
L01735:
; Line 298
	jmp	L01732
; Line 299
L01734:
L01731:
; Line 299
	add	WORD PTR -4[bp],OFFSET 1
L01733:
	cmp	WORD PTR -4[bp],OFFSET 4
	jl	L01730
L01732:
; Line 300
	cmp	WORD PTR -4[bp],OFFSET 4
	je	L01736
	cmp	WORD PTR -590[bp],OFFSET 0
	je	L01736
; Line 301
; Line 302
	mov	ax,WORD PTR -4[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -588[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	WORD PTR -598[bp],ax
	mov	WORD PTR -596[bp],dx
; Line 303
L01738:
	mov	ax,WORD PTR -594[bp]
	cmp	ax,WORD PTR -590[bp]
	ja	L01739
; Line 304
; Line 305
	push	OFFSET 16
	_imul3 	ax,WORD PTR -594[bp],OFFSET 20
	lea	cx,WORD PTR -524[bp]
	add	cx,ax
	mov	ax,OFFSET 4
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	mov	ax,OFFSET 68
	mov	cx,WORD PTR 8[bp]
	mov	dx,WORD PTR 10[bp]
	add	cx,ax
	_imul3 	ax,WORD PTR -594[bp],OFFSET 28
	add	cx,ax
	mov	ax,OFFSET 12
	add	cx,ax
	push	dx
	push	cx
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 306
	cmp	WORD PTR -594[bp],OFFSET 0
	je	L02334
	mov	ax,WORD PTR -594[bp]
	add	ax,OFFSET -1
	_imul3 	ax,ax,OFFSET 20
	lea	bx,WORD PTR -524[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:[bx]
	mov	dx,WORD PTR ss:2[bx]
	jmp	L02333
L02334:
	mov	ax,WORD PTR -598[bp]
	mov	dx,WORD PTR -596[bp]
L02333:
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 8[bp]
	mov	si,WORD PTR 10[bp]
	add	bx,cx
	mov	WORD PTR -602[bp],ax
	mov	WORD PTR -600[bp],dx
	_imul3 	ax,WORD PTR -594[bp],OFFSET 28
	add	bx,ax
	mov	es,si
	mov	ax,WORD PTR -602[bp]
	mov	dx,WORD PTR -600[bp]
	mov	WORD PTR es:[bx],ax
	mov	WORD PTR es:2[bx],dx
; Line 307
	mov	ax,WORD PTR -594[bp]
	add	ax,OFFSET 1
	_imul3 	ax,ax,OFFSET 20
	lea	bx,WORD PTR -524[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:[bx]
	mov	dx,WORD PTR ss:2[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 8[bp]
	mov	si,WORD PTR 10[bp]
	add	bx,cx
	mov	WORD PTR -602[bp],ax
	mov	WORD PTR -600[bp],dx
	_imul3 	ax,WORD PTR -594[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -602[bp]
	mov	dx,WORD PTR -600[bp]
	mov	es,si
	mov	WORD PTR es:4[bx],ax
	mov	WORD PTR es:6[bx],dx
; Line 308
	_imul3 	ax,WORD PTR -594[bp],OFFSET 20
	lea	bx,WORD PTR -524[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:12[bx]
	mov	dx,WORD PTR ss:14[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 8[bp]
	mov	si,WORD PTR 10[bp]
	add	bx,cx
	mov	WORD PTR -602[bp],ax
	mov	WORD PTR -600[bp],dx
	_imul3 	ax,WORD PTR -594[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -602[bp]
	mov	dx,WORD PTR -600[bp]
	mov	es,si
	mov	WORD PTR es:8[bx],ax
	mov	WORD PTR es:10[bx],dx
; Line 309
	_imul3 	ax,WORD PTR -594[bp],OFFSET 20
	lea	bx,WORD PTR -524[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:[bx]
	mov	dx,WORD PTR ss:2[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 8[bp]
	mov	si,WORD PTR 10[bp]
	add	bx,cx
	mov	WORD PTR -602[bp],ax
	mov	WORD PTR -600[bp],dx
	_imul3 	ax,WORD PTR -594[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -602[bp]
	mov	dx,WORD PTR -600[bp]
	mov	es,si
	mov	WORD PTR es:20[bx],ax
	mov	WORD PTR es:22[bx],dx
; Line 310
	add	WORD PTR -594[bp],OFFSET 1
; Line 311
; Line 311
	jmp	L01738
L01739:
; Line 312
; Line 313
L01736:
	mov	ax,OFFSET 1
	jmp	L01722
; Line 314
; Line 314
L01722:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 317
; dwStaSec = 0006
; wSectors = 000a
; pBuf = 000c
; btHardDisk = 0010
; pDriveParam = 0012
_ReadSector:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02335
	call	FAR PTR __aFchkstk
	push	si
	push	di
; dapDisk = ffee
; wSeg = ffec
; i = ffea
; btResult = ffe8
; k = ffe6
; wOffset = ffe4
; wCount = ffe2
; dwPhySec = ffde
; pdapDisk = ffda
; wOff = ffd8
; Line 318
; Line 319
; Line 320
	mov	WORD PTR -28[bp],OFFSET 0
; Line 321
	mov	BYTE PTR -24[bp],OFFSET 1
; Line 322
; Line 323
	mov	al,BYTE PTR 16[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _IsSupportExt13
	add	sp,OFFSET 2
	mov	WORD PTR ?blSupportExt13@@3HE,ax
; Line 324
	mov	cx,OFFSET 7
	mov	ax,WORD PTR 10[bp]
	sub	dx,dx
	div	cx
	add	ax,OFFSET 1
	mov	WORD PTR -22[bp],ax
; Line 325
	lea	ax,WORD PTR -18[bp]
	mov	dx,ss
	mov	WORD PTR -38[bp],ax
	mov	WORD PTR -36[bp],dx
; Line 326
	mov	WORD PTR -26[bp],OFFSET 0
	jmp	L01761
L01758:
; Line 327
; Line 328
	mov	ax,WORD PTR -28[bp]
	add	ax,OFFSET 7
	cmp	ax,WORD PTR 10[bp]
	jbe	L01762
; Line 329
	mov	ax,WORD PTR 10[bp]
	sub	ax,WORD PTR -28[bp]
	mov	WORD PTR -30[bp],ax
; Line 330
	jmp	L01763
L01762:
; Line 330
	mov	WORD PTR -30[bp],OFFSET 7
L01763:
; Line 332
	cmp	WORD PTR -30[bp],OFFSET 0
	jne	L01764
	jmp	L01760
; Line 333
L01764:
	cmp	WORD PTR ?blSupportExt13@@3HE,OFFSET 0
	je	L01765
; Line 334
; Line 335
	mov	BYTE PTR -18[bp],OFFSET 16
; Line 336
	mov	BYTE PTR -17[bp],OFFSET 0
; Line 337
	mov	ax,WORD PTR -30[bp]
	mov	WORD PTR -16[bp],ax
; Line 338
	mov	ax,WORD PTR -28[bp]
	shl	ax,OFFSET 9
	mov	dx,OFFSET 0
	mov	cx,WORD PTR 12[bp]
	mov	bx,WORD PTR 14[bp]
	add	cx,ax
	adc	bx,dx
	mov	WORD PTR -14[bp],cx
	mov	WORD PTR -12[bp],bx
; Line 339
	mov	ax,WORD PTR -28[bp]
	mov	dx,OFFSET 0
	add	ax,WORD PTR 6[bp]
	adc	dx,WORD PTR 8[bp]
	mov	WORD PTR -10[bp],ax
	mov	WORD PTR -8[bp],dx
; Line 340
	mov	WORD PTR -6[bp],OFFSET 0
	mov	WORD PTR -4[bp],OFFSET 0
; Line 341
	lea	bx,WORD PTR -38[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR -40[bp],ax
; Line 342
	lea	bx,WORD PTR -38[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:2[bx]
	mov	WORD PTR -20[bp],ax
; Line 343
; Line 344
; Line 345
	push	ds
; Line 346
	mov	ah,OFFSET 66
; Line 347
	mov	dl,BYTE PTR 16[bp]
; Line 348
	mov	si,WORD PTR -40[bp]
; Line 349
	mov	ds,WORD PTR -20[bp]
; Line 350
	int	OFFSET 19
; Line 351
	mov	BYTE PTR -24[bp],ah
; Line 352
	pop	ds
; Line 353
; Line 354
	mov	al,BYTE PTR -24[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01770
	mov	ax,OFFSET 0
	jmp	L01747
; Line 355
L01770:
; Line 356
	jmp	L01771
L01765:
; Line 356
; Line 357
; d1 = ffd6
; wAx = ffd4
; c1 = ffd2
; Line 358
; Line 359
	push	WORD PTR 20[bp]
	push	WORD PTR 18[bp]
	mov	ax,WORD PTR -28[bp]
	mov	dx,OFFSET 0
	add	ax,WORD PTR 6[bp]
	adc	dx,WORD PTR 8[bp]
	push	dx
	push	ax
	call	FAR PTR _LogicToPhy
	add	sp,OFFSET 8
	mov	WORD PTR -34[bp],ax
	mov	WORD PTR -32[bp],dx
; Line 360
	mov	cx,OFFSET 16
	mov	ax,WORD PTR -34[bp]
	mov	dx,WORD PTR -32[bp]
	call	FAR PTR __aFulshr
	and	ax,OFFSET -1
	and	dx,OFFSET 0
	mov	WORD PTR -46[bp],ax
; Line 361
	mov	ax,WORD PTR -30[bp]
	add	ax,OFFSET 512
	mov	WORD PTR -44[bp],ax
; Line 362
	mov	al,BYTE PTR 16[bp]
	and	ax,OFFSET 255
	mov	cx,WORD PTR -34[bp]
	mov	dx,WORD PTR -32[bp]
	and	cx,OFFSET -256
	add	cx,ax
	mov	WORD PTR -42[bp],cx
; Line 363
	mov	ax,WORD PTR -28[bp]
	shl	ax,OFFSET 9
	mov	dx,OFFSET 0
	mov	cx,WORD PTR 12[bp]
	mov	bx,WORD PTR 14[bp]
	add	cx,ax
	adc	bx,dx
	mov	WORD PTR -40[bp],cx
; Line 364
; Line 365
; Line 366
	mov	ax,WORD PTR -44[bp]
; Line 367
	mov	dx,WORD PTR -42[bp]
; Line 368
	mov	cx,WORD PTR -46[bp]
; Line 369
	mov	bx,WORD PTR -40[bp]
; Line 370
	int	OFFSET 19
; Line 371
	mov	BYTE PTR -24[bp],ah
; Line 372
; Line 373
	mov	al,BYTE PTR -24[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01780
	mov	ax,OFFSET 0
	jmp	L01747
; Line 374
L01780:
L01771:
; Line 375
	mov	ax,WORD PTR -30[bp]
	add	WORD PTR -28[bp],ax
; Line 376
L01759:
; Line 376
	add	WORD PTR -26[bp],OFFSET 1
L01761:
	mov	ax,WORD PTR -22[bp]
	cmp	WORD PTR -26[bp],ax
	jl	L01758
L01760:
; Line 377
	mov	al,BYTE PTR -24[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	jne	L02338
	mov	ax,OFFSET 1
	jmp	L02337
L02338:
	mov	ax,OFFSET 0
L02337:
	jmp	L01747
; Line 378
; Line 378
L01747:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 381
; dwLogicSec = 0006
; pDiskParam = 000a
_LogicToPhy:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02339
	call	FAR PTR __aFchkstk
	push	si
	push	di
; dwHeader = fffa
; dwCylinder = fff6
; dwSector = fff2
; Line 382
; Line 383
; Line 384
; Line 386
	les	bx,WORD PTR 10[bp]
	push	WORD PTR es:14[bx]
	push	WORD PTR es:12[bx]
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	call	FAR PTR __aFulrem
	add	ax,OFFSET 1
	adc	dx,OFFSET 0
	mov	WORD PTR -14[bp],ax
	mov	WORD PTR -12[bp],dx
; Line 387
; Line 388
	les	bx,WORD PTR 10[bp]
	push	WORD PTR es:10[bx]
	push	WORD PTR es:8[bx]
	les	bx,WORD PTR 10[bp]
	push	WORD PTR es:14[bx]
	push	WORD PTR es:12[bx]
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	call	FAR PTR __aFuldiv
	push	dx
	push	ax
	call	FAR PTR __aFulrem
	mov	WORD PTR -6[bp],ax
	mov	WORD PTR -4[bp],dx
; Line 389
; Line 390
	les	bx,WORD PTR 10[bp]
	push	WORD PTR es:10[bx]
	push	WORD PTR es:8[bx]
	les	bx,WORD PTR 10[bp]
	push	WORD PTR es:14[bx]
	push	WORD PTR es:12[bx]
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	call	FAR PTR __aFuldiv
	push	dx
	push	ax
	call	FAR PTR __aFuldiv
	mov	WORD PTR -10[bp],ax
	mov	WORD PTR -8[bp],dx
; Line 391
	mov	cx,OFFSET 16
	mov	ax,WORD PTR -14[bp]
	mov	dx,WORD PTR -12[bp]
	call	FAR PTR __aFlshl
	mov	cx,OFFSET 8
	mov	WORD PTR -18[bp],ax
	mov	WORD PTR -16[bp],dx
	mov	ax,WORD PTR -6[bp]
	mov	dx,WORD PTR -4[bp]
	call	FAR PTR __aFlshl
	mov	cx,WORD PTR -10[bp]
	mov	bx,WORD PTR -8[bp]
	and	cx,OFFSET 768
	and	bx,OFFSET 0
	mov	WORD PTR -22[bp],ax
	mov	WORD PTR -20[bp],dx
	mov	ax,cx
	mov	dx,bx
	mov	cx,OFFSET 14
	call	FAR PTR __aFlshl
	mov	cx,WORD PTR -10[bp]
	mov	bx,WORD PTR -8[bp]
	and	cx,OFFSET 255
	and	bx,OFFSET 0
	mov	WORD PTR -26[bp],ax
	mov	WORD PTR -24[bp],dx
	mov	ax,cx
	mov	dx,bx
	mov	cx,OFFSET 24
	call	FAR PTR __aFlshl
	mov	cx,WORD PTR -26[bp]
	mov	bx,WORD PTR -24[bp]
	or	ax,cx
	or	dx,bx
	mov	cx,WORD PTR -22[bp]
	mov	bx,WORD PTR -20[bp]
	or	ax,cx
	or	dx,bx
	mov	cx,WORD PTR -18[bp]
	mov	bx,WORD PTR -16[bp]
	or	ax,cx
	or	dx,bx
	jmp	L01784
; Line 392
; Line 392
L01784:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 396
_GetHardDiskNum:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02341
	call	FAR PTR __aFchkstk
	push	si
	push	di
; btResult = fffc
; Line 397
	mov	BYTE PTR -4[bp],OFFSET 0
; Line 398
; Line 399
; Line 400
	mov	ah,OFFSET 8
; Line 401
	mov	dl,OFFSET 128
; Line 402
	int	OFFSET 19
; Line 403
	mov	BYTE PTR -4[bp],dl
; Line 404
; Line 405
	mov	al,BYTE PTR -4[bp]
	and	ax,OFFSET 255
	jmp	L01789
; Line 406
; Line 406
L01789:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 409
; dwStartSec = 0006
; btPriOrLog = 000a
; btHardDrive = 000c
; blHideOrShow = 000e
_HidePartition:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02342
	call	FAR PTR __aFchkstk
	push	si
	push	di
; PartitionMBR = fdfe
; DriveParam = fde4
; Line 410
; Line 411
; Line 413
	lea	ax,WORD PTR -540[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetDriveParam
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L01798
; Line 414
	mov	ax,OFFSET 0
	jmp	L01795
; Line 415
L01798:
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 1
	jne	L01799
; Line 416
; i = fde2
; Line 417
	lea	ax,WORD PTR -540[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01800
; Line 418
	mov	ax,OFFSET 0
	jmp	L01795
; Line 419
L01800:
	mov	WORD PTR -542[bp],OFFSET 0
	jmp	L01805
L01802:
; Line 420
; Line 421
	mov	ax,WORD PTR -542[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -68[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	cmp	WORD PTR 6[bp],ax
	jne	L01806
	cmp	WORD PTR 8[bp],dx
	jne	L01806
; Line 422
; Line 423
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 128
	jne	L01807
; Line 424
; Line 425
	mov	ax,WORD PTR -542[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -68[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 128
	je	L01808
; Line 426
; Line 427
	mov	al,BYTE PTR 14[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01809
; Line 428
	mov	ax,WORD PTR -542[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -68[bp]
	add	bx,ax
	add	BYTE PTR ss:4[bx],OFFSET 16
; Line 429
	jmp	L01810
L01809:
; Line 429
; Line 430
	mov	ax,WORD PTR -542[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -68[bp]
	add	bx,ax
	sub	BYTE PTR ss:4[bx],OFFSET 16
L01810:
; Line 431
	lea	ax,WORD PTR -540[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	jmp	L01795
; Line 432
; Line 433
L01808:
; Line 434
	jmp	L01811
L01807:
; Line 434
; Line 435
; Line 436
	mov	al,BYTE PTR 14[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01812
; Line 437
	mov	ax,WORD PTR -542[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -68[bp]
	add	bx,ax
	add	BYTE PTR ss:4[bx],OFFSET 16
; Line 438
	jmp	L01813
L01812:
; Line 438
; Line 439
	mov	ax,WORD PTR -542[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -68[bp]
	add	bx,ax
	sub	BYTE PTR ss:4[bx],OFFSET 16
L01813:
; Line 440
	lea	ax,WORD PTR -540[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	jmp	L01795
; Line 441
L01811:
; Line 442
; Line 443
L01806:
L01803:
; Line 443
	add	WORD PTR -542[bp],OFFSET 1
L01805:
	cmp	WORD PTR -542[bp],OFFSET 4
	jl	L01802
L01804:
; Line 444
; Line 445
L01799:
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 3
	jne	L01814
; Line 446
; Line 447
	mov	ax,WORD PTR 6[bp]
	mov	dx,WORD PTR 8[bp]
	mov	WORD PTR 6[bp],ax
	mov	WORD PTR 8[bp],dx
; Line 448
	lea	ax,WORD PTR -540[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01815
; Line 449
	mov	ax,OFFSET 0
	jmp	L01795
; Line 450
L01815:
	mov	al,BYTE PTR 14[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01816
; Line 451
	add	BYTE PTR -64[bp],OFFSET 16
; Line 452
	jmp	L01817
L01816:
; Line 452
; Line 453
	sub	BYTE PTR -64[bp],OFFSET 16
L01817:
; Line 454
	lea	ax,WORD PTR -540[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 12[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	jmp	L01795
; Line 455
; Line 456
L01814:
	mov	ax,OFFSET 0
	jmp	L01795
; Line 457
; Line 457
L01795:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 460
; dwStartSec = 0006
; btHardDrive = 000a
_ActivePartition:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02343
	call	FAR PTR __aFchkstk
	push	si
	push	di
; j = fffc
; i = fffa
; PartitionMBR = fdfa
; DriveParam = fde0
; Line 461
; Line 462
; Line 463
; Line 464
	lea	ax,WORD PTR -544[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetDriveParam
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L01824
; Line 465
	mov	ax,OFFSET 0
	jmp	L01820
; Line 466
L01824:
	lea	ax,WORD PTR -544[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -518[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01825
; Line 467
	mov	ax,OFFSET 0
	jmp	L01820
; Line 468
L01825:
	mov	WORD PTR -4[bp],OFFSET 0
	jmp	L01829
L01826:
; Line 469
; Line 470
	mov	ax,WORD PTR -4[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -72[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 128
	jne	L01830
; Line 471
	jmp	L01828
; Line 472
L01830:
L01827:
; Line 472
	add	WORD PTR -4[bp],OFFSET 1
L01829:
	cmp	WORD PTR -4[bp],OFFSET 4
	jl	L01826
L01828:
; Line 473
	mov	WORD PTR -6[bp],OFFSET 0
	jmp	L01835
L01832:
; Line 474
; Line 475
	mov	ax,WORD PTR -6[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -72[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	cmp	WORD PTR 6[bp],ax
	jne	L01836
	cmp	WORD PTR 8[bp],dx
	jne	L01836
; Line 476
; Line 477
	mov	ax,WORD PTR -4[bp]
	cmp	WORD PTR -6[bp],ax
	jne	L01837
; Line 478
	mov	ax,OFFSET 1
	jmp	L01820
; Line 479
	jmp	L01838
L01837:
; Line 479
; Line 480
; Line 481
	mov	ax,WORD PTR -6[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -72[bp]
	add	bx,ax
	mov	BYTE PTR ss:[bx],OFFSET 128
; Line 482
	cmp	WORD PTR -4[bp],OFFSET 4
	jge	L01839
; Line 483
	mov	ax,WORD PTR -4[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -72[bp]
	add	bx,ax
	mov	BYTE PTR ss:[bx],OFFSET 0
; Line 484
L01839:
L01838:
; Line 485
	lea	ax,WORD PTR -544[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -518[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	jmp	L01820
; Line 486
; Line 487
L01836:
L01833:
; Line 487
	add	WORD PTR -6[bp],OFFSET 1
L01835:
	cmp	WORD PTR -6[bp],OFFSET 4
	jl	L01832
L01834:
; Line 488
	mov	ax,OFFSET 0
	jmp	L01820
; Line 489
; Line 489
L01820:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 492
; dwStaSec = 0006
; wSectors = 000a
; pBuf = 000c
; btHardDisk = 0010
; pDriveParam = 0012
_WriteSector:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02344
	call	FAR PTR __aFchkstk
	push	si
	push	di
; dapDisk = ffee
; wSeg = ffec
; i = ffea
; btResult = ffe8
; k = ffe6
; wOffset = ffe4
; btVerify = ffe2
; wCount = ffe0
; dwPhySec = ffdc
; pdapDisk = ffd8
; wOff = ffd6
; Line 493
; Line 494
; Line 495
	mov	WORD PTR -28[bp],OFFSET 0
; Line 496
	mov	BYTE PTR -24[bp],OFFSET 1
	mov	BYTE PTR -30[bp],OFFSET 0
; Line 497
; Line 499
	mov	al,BYTE PTR 16[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _IsSupportExt13
	add	sp,OFFSET 2
	mov	WORD PTR ?blSupportExt13@@3HE,ax
; Line 500
	mov	cx,OFFSET 7
	mov	ax,WORD PTR 10[bp]
	sub	dx,dx
	div	cx
	add	ax,OFFSET 1
	mov	WORD PTR -22[bp],ax
; Line 501
	lea	ax,WORD PTR -18[bp]
	mov	dx,ss
	mov	WORD PTR -40[bp],ax
	mov	WORD PTR -38[bp],dx
; Line 502
	mov	WORD PTR -26[bp],OFFSET 0
	jmp	L01860
L01857:
; Line 503
; Line 504
	mov	ax,WORD PTR -28[bp]
	add	ax,OFFSET 7
	cmp	ax,WORD PTR 10[bp]
	jbe	L01861
; Line 505
	mov	ax,WORD PTR 10[bp]
	sub	ax,WORD PTR -28[bp]
	mov	WORD PTR -32[bp],ax
; Line 506
	jmp	L01862
L01861:
; Line 506
	mov	WORD PTR -32[bp],OFFSET 7
L01862:
; Line 508
	cmp	WORD PTR -32[bp],OFFSET 0
	jne	L01863
	jmp	L01859
; Line 509
L01863:
	cmp	WORD PTR ?blSupportExt13@@3HE,OFFSET 0
	je	L01864
; Line 510
; Line 511
	les	bx,WORD PTR 18[bp]
	mov	ax,WORD PTR es:2[bx]
	and	ax,OFFSET 4
	cmp	ax,OFFSET 0
	je	L01865
; Line 512
	mov	BYTE PTR -30[bp],OFFSET 1
; Line 513
L01865:
	mov	BYTE PTR -18[bp],OFFSET 16
; Line 514
	mov	BYTE PTR -17[bp],OFFSET 0
; Line 515
	mov	ax,WORD PTR -32[bp]
	mov	WORD PTR -16[bp],ax
; Line 516
	mov	ax,WORD PTR -28[bp]
	shl	ax,OFFSET 9
	mov	dx,OFFSET 0
	mov	cx,WORD PTR 12[bp]
	mov	bx,WORD PTR 14[bp]
	add	cx,ax
	adc	bx,dx
	mov	WORD PTR -14[bp],cx
	mov	WORD PTR -12[bp],bx
; Line 517
	mov	ax,WORD PTR -28[bp]
	mov	dx,OFFSET 0
	add	ax,WORD PTR 6[bp]
	adc	dx,WORD PTR 8[bp]
	mov	WORD PTR -10[bp],ax
	mov	WORD PTR -8[bp],dx
; Line 518
	mov	WORD PTR -6[bp],OFFSET 0
	mov	WORD PTR -4[bp],OFFSET 0
; Line 519
	lea	bx,WORD PTR -40[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR -42[bp],ax
; Line 520
	lea	bx,WORD PTR -40[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:2[bx]
	mov	WORD PTR -20[bp],ax
; Line 521
; Line 522
; Line 523
	push	ds
; Line 524
	mov	ah,OFFSET 67
; Line 525
	mov	al,BYTE PTR -30[bp]
; Line 526
	mov	dl,BYTE PTR 16[bp]
; Line 527
	mov	si,WORD PTR -42[bp]
; Line 528
	mov	ds,WORD PTR -20[bp]
; Line 529
	stc
; Line 530
	int	OFFSET 19
; Line 531
	jb	WRITE_FAIL
; Line 532
	mov	BYTE PTR -24[bp],ah
; Line 533
	pop	ds
; Line 534
; Line 535
WRITE_FAIL:
; Line 536
	mov	al,BYTE PTR -24[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01871
	mov	ax,OFFSET 0
	jmp	L01845
; Line 537
L01871:
; Line 538
	jmp	L01872
L01864:
; Line 538
; Line 539
; d1 = ffd4
; wAx = ffd2
; c1 = ffd0
; Line 540
; Line 541
	push	WORD PTR 20[bp]
	push	WORD PTR 18[bp]
	mov	ax,WORD PTR -28[bp]
	mov	dx,OFFSET 0
	add	ax,WORD PTR 6[bp]
	adc	dx,WORD PTR 8[bp]
	push	dx
	push	ax
	call	FAR PTR _LogicToPhy
	add	sp,OFFSET 8
	mov	WORD PTR -36[bp],ax
	mov	WORD PTR -34[bp],dx
; Line 542
	mov	cx,OFFSET 16
	mov	ax,WORD PTR -36[bp]
	mov	dx,WORD PTR -34[bp]
	call	FAR PTR __aFulshr
	and	ax,OFFSET -1
	and	dx,OFFSET 0
	mov	WORD PTR -48[bp],ax
; Line 543
	mov	ax,WORD PTR -32[bp]
	add	ax,OFFSET 768
	mov	WORD PTR -46[bp],ax
; Line 544
	mov	al,BYTE PTR 16[bp]
	and	ax,OFFSET 255
	mov	cx,WORD PTR -36[bp]
	mov	dx,WORD PTR -34[bp]
	and	cx,OFFSET -256
	add	cx,ax
	mov	WORD PTR -44[bp],cx
; Line 545
	mov	ax,WORD PTR -28[bp]
	shl	ax,OFFSET 9
	mov	dx,OFFSET 0
	mov	cx,WORD PTR 12[bp]
	mov	bx,WORD PTR 14[bp]
	add	cx,ax
	adc	bx,dx
	mov	WORD PTR -42[bp],cx
; Line 546
; Line 547
; Line 548
	mov	ax,WORD PTR -46[bp]
; Line 549
	mov	dx,WORD PTR -44[bp]
; Line 550
	mov	cx,WORD PTR -48[bp]
; Line 551
	mov	bx,WORD PTR -42[bp]
; Line 552
	int	OFFSET 19
; Line 553
	mov	BYTE PTR -24[bp],ah
; Line 554
; Line 555
	mov	al,BYTE PTR -24[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01881
	mov	ax,OFFSET 0
	jmp	L01845
; Line 556
L01881:
L01872:
; Line 557
	mov	ax,WORD PTR -32[bp]
	add	WORD PTR -28[bp],ax
; Line 558
L01858:
; Line 558
	add	WORD PTR -26[bp],OFFSET 1
L01860:
	mov	ax,WORD PTR -22[bp]
	cmp	WORD PTR -26[bp],ax
	jl	L01857
L01859:
; Line 559
	mov	al,BYTE PTR -24[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	jne	L02346
	mov	ax,OFFSET 1
	jmp	L02345
L02346:
	mov	ax,OFFSET 0
L02345:
	jmp	L01845
; Line 560
; Line 560
L01845:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 563
; dwStartSector = 0006
; btHardDisk = 000a
; flags = 000c
; pnErr = 0010
_DoDeletePartition:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02347
	call	FAR PTR __aFchkstk
	push	si
	push	di
; PartitionLogicalMBR = fdfe
; i = fdfc
; PartitionMBR = fbfc
; dwStartSec = fbf8
; DriveParam = fbde
; dwPreStart = fbda
; ParHard = f8be
; Line 564
; Line 565
; Line 566
; Line 567
; Line 568
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 0
; Line 569
; Line 571
	lea	ax,WORD PTR -1058[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetDriveParam
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L01895
; Line 572
	mov	ax,OFFSET 0
	jmp	L01887
; Line 574
L01895:
	lea	ax,WORD PTR -1058[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -1028[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01896
; Line 575
; Line 576
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 103
; Line 577
	mov	ax,OFFSET 0
	jmp	L01887
; Line 578
; Line 579
L01896:
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 128
	jle	L01897
	mov	ax,WORD PTR -1056[bp]
	and	ax,OFFSET 4
	cmp	ax,OFFSET 0
	je	L01897
	cmp	WORD PTR 6[bp],OFFSET 0
	jne	L01897
	cmp	WORD PTR 8[bp],OFFSET 0
	jne	L01897
; Line 580
; Line 581
	lea	ax,WORD PTR -1058[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	mov	ax,OFFSET StdMBR
	mov	dx,ds
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01899
; Line 582
; Line 583
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 584
	mov	ax,OFFSET 0
	jmp	L01887
; Line 585
; Line 586
L01899:
	lea	ax,WORD PTR -1058[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	mov	ax,OFFSET StdMBR
	mov	dx,ds
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 6
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01901
; Line 587
; Line 588
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 589
	mov	ax,OFFSET 0
	jmp	L01887
; Line 590
; Line 591
L01901:
	mov	ax,OFFSET 1
	jmp	L01887
; Line 592
; Line 593
L01897:
	mov	WORD PTR -516[bp],OFFSET 0
	jmp	L01905
L01902:
; Line 594
; Line 595
; Line 596
; Line 597
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	cmp	WORD PTR 6[bp],ax
	jne	L01906
	cmp	WORD PTR 8[bp],dx
	jne	L01906
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 5
	je	L01906
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 15
	je	L01906
; Line 598
	jmp	L01904
; Line 599
L01906:
L01903:
; Line 599
	add	WORD PTR -516[bp],OFFSET 1
L01905:
	cmp	WORD PTR -516[bp],OFFSET 4
	jl	L01902
L01904:
; Line 600
; Line 601
	cmp	WORD PTR -516[bp],OFFSET 4
	je	L01907
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 5
	je	L01907
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 15
	je	L01907
; Line 602
; Line 603
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 128
	jne	L01908
	mov	cx,OFFSET 1
	mov	ax,WORD PTR 12[bp]
	mov	dx,WORD PTR 14[bp]
	call	FAR PTR __aFulshr
	and	ax,OFFSET 1
	and	dx,OFFSET 0
	cmp	ax,OFFSET 0
	jne	L01908
	cmp	dx,OFFSET 0
	jne	L01908
; Line 604
	mov	ax,OFFSET 0
	jmp	L01887
; Line 605
L01908:
	push	OFFSET 16
	push	OFFSET 0
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	cx,WORD PTR -582[bp]
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 606
	lea	ax,WORD PTR -1058[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -1028[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01910
; Line 607
; Line 608
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 609
	mov	ax,OFFSET 0
	jmp	L01887
; Line 610
; Line 611
L01910:
	mov	ax,OFFSET 1
	jmp	L01887
; Line 612
; Line 613
L01907:
	mov	WORD PTR -516[bp],OFFSET 0
	jmp	L01914
L01911:
; Line 614
; Line 615
; Line 616
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 5
	je	L01916
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 15
	jne	L01915
L01916:
; Line 617
	jmp	L01913
; Line 618
L01915:
L01912:
; Line 618
	add	WORD PTR -516[bp],OFFSET 1
L01914:
	cmp	WORD PTR -516[bp],OFFSET 4
	jl	L01911
L01913:
; Line 619
	cmp	WORD PTR -516[bp],OFFSET 4
	je	L01917
	mov	ax,WORD PTR 12[bp]
	mov	dx,WORD PTR 14[bp]
	and	ax,OFFSET 1
	and	dx,OFFSET 0
	cmp	ax,OFFSET 0
	jne	L01917
	cmp	dx,OFFSET 0
	jne	L01917
; Line 620
; Line 621
	push	OFFSET 16
	push	OFFSET 0
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	cx,WORD PTR -582[bp]
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 622
	lea	ax,WORD PTR -1058[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -1028[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01919
; Line 623
; Line 624
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 625
	mov	ax,OFFSET 0
	jmp	L01887
; Line 626
; Line 627
L01919:
	mov	ax,OFFSET 1
	jmp	L01887
; Line 628
; Line 629
L01917:
	lea	ax,WORD PTR -1858[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetPartitionInfoEx
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L01920
; Line 630
; Line 631
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 106
; Line 632
	mov	ax,OFFSET 0
	jmp	L01887
; Line 633
; Line 634
L01920:
	cmp	WORD PTR -516[bp],OFFSET 4
	je	L01921
	cmp	WORD PTR -1856[bp],OFFSET 0
	je	L01921
; Line 635
; Line 636
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	WORD PTR -1032[bp],ax
	mov	WORD PTR -1030[bp],dx
; Line 637
	lea	ax,WORD PTR -1058[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR -1030[bp]
	push	WORD PTR -1032[bp]
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01922
; Line 638
; Line 639
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 103
; Line 640
	mov	ax,OFFSET 0
	jmp	L01887
; Line 641
; Line 642
L01922:
	mov	ax,WORD PTR -1032[bp]
	mov	dx,WORD PTR -1030[bp]
	cmp	WORD PTR 6[bp],ax
	jne	L01923
	cmp	WORD PTR 8[bp],dx
	jne	L01923
; Line 643
; Line 644
	push	OFFSET 16
	push	OFFSET 0
	lea	ax,WORD PTR -68[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 645
	lea	ax,WORD PTR -1058[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR -1030[bp]
	push	WORD PTR -1032[bp]
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01925
; Line 646
; Line 647
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 648
	mov	ax,OFFSET 0
	jmp	L01887
; Line 649
; Line 650
L01925:
	mov	ax,OFFSET 1
	jmp	L01887
; Line 651
; Line 652
	jmp	L01926
L01923:
; Line 652
; Line 653
; Line 654
	mov	WORD PTR -516[bp],OFFSET 0
	jmp	L01927
L01928:
	add	WORD PTR -516[bp],OFFSET 1
L01927:
	mov	ax,WORD PTR -516[bp]
	cmp	ax,WORD PTR -1856[bp]
	jae	L01929
; Line 655
; Line 656
	_imul3 	ax,WORD PTR -516[bp],OFFSET 28
	lea	bx,WORD PTR -1790[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:20[bx]
	mov	dx,WORD PTR ss:22[bx]
	cmp	WORD PTR 6[bp],ax
	jne	L01931
	cmp	WORD PTR 8[bp],dx
	jne	L01931
; Line 657
	jmp	L01929
; Line 658
L01931:
; Line 658
	jmp	L01928
L01929:
; Line 659
	mov	ax,WORD PTR -516[bp]
	cmp	ax,WORD PTR -1856[bp]
	jne	L01932
; Line 660
; Line 661
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 111
; Line 662
	mov	ax,OFFSET 0
	jmp	L01887
; Line 663
; Line 664
L01932:
	_imul3 	ax,WORD PTR -516[bp],OFFSET 28
	lea	bx,WORD PTR -1790[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:[bx]
	mov	dx,WORD PTR ss:2[bx]
	mov	WORD PTR -1062[bp],ax
	mov	WORD PTR -1060[bp],dx
; Line 665
	lea	ax,WORD PTR -1058[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01933
; Line 666
; Line 667
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 103
; Line 668
	mov	ax,OFFSET 0
	jmp	L01887
; Line 669
; Line 670
L01933:
	lea	ax,WORD PTR -1058[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -1028[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR -1060[bp]
	push	WORD PTR -1062[bp]
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01934
; Line 671
; Line 672
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 103
; Line 673
	mov	ax,OFFSET 0
	jmp	L01887
; Line 674
; Line 675
	jmp	L01935
L01934:
; Line 675
; Line 676
; Line 677
	push	OFFSET 16
	lea	ax,WORD PTR -52[bp]
	mov	dx,ss
	push	dx
	push	ax
	lea	ax,WORD PTR -566[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 678
	lea	ax,WORD PTR -1058[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -1028[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR -1060[bp]
	push	WORD PTR -1062[bp]
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01937
; Line 679
; Line 680
	les	bx,WORD PTR 16[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 681
	mov	ax,OFFSET 0
	jmp	L01887
; Line 682
; Line 683
L01937:
	mov	ax,OFFSET 1
	jmp	L01887
; Line 684
L01935:
; Line 685
L01926:
; Line 686
; Line 687
L01921:
	mov	ax,OFFSET 0
	jmp	L01887
; Line 688
; Line 688
L01887:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 691
; ph = 0006
; pe = 000a
_VerifyLogicalPartition:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02348
	call	FAR PTR __aFchkstk
	push	si
	push	di
; j = fffc
; i = fffa
; bl = fff8
; Line 692
	mov	WORD PTR -6[bp],OFFSET 0
	mov	WORD PTR -4[bp],OFFSET 0
; Line 693
	mov	WORD PTR -8[bp],OFFSET 0
; Line 694
	mov	WORD PTR -6[bp],OFFSET 0
	jmp	L01944
L01945:
	add	WORD PTR -6[bp],OFFSET 1
L01944:
	mov	ax,WORD PTR -6[bp]
	les	bx,WORD PTR 6[bp]
	cmp	ax,WORD PTR es:2[bx]
	jae	L01946
; Line 695
; Line 696
; Line 697
	mov	ax,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	cx,WORD PTR 8[bp]
	add	bx,ax
	mov	ax,WORD PTR -6[bp]
	add	ax,OFFSET 1
	_imul3 	ax,ax,OFFSET 28
	add	bx,ax
	mov	es,cx
	mov	ax,WORD PTR es:20[bx]
	mov	dx,WORD PTR es:22[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	WORD PTR -12[bp],ax
	mov	WORD PTR -10[bp],dx
	_imul3 	ax,WORD PTR -6[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -12[bp]
	mov	dx,WORD PTR -10[bp]
	mov	es,si
	cmp	WORD PTR es:22[bx],dx
	jb	L01948
	ja	L02349
	cmp	WORD PTR es:20[bx],ax
	jb	L01948
L02349:
	mov	ax,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	cx,WORD PTR 8[bp]
	add	bx,ax
	_imul3 	ax,WORD PTR -6[bp],OFFSET 28
	add	bx,ax
	mov	es,cx
	mov	ax,WORD PTR es:20[bx]
	mov	dx,WORD PTR es:22[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	WORD PTR -12[bp],ax
	mov	WORD PTR -10[bp],dx
	_imul3 	ax,WORD PTR -6[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -12[bp]
	mov	dx,WORD PTR -10[bp]
	mov	es,si
	add	ax,WORD PTR es:24[bx]
	adc	dx,WORD PTR es:26[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	cx,WORD PTR -6[bp]
	add	cx,OFFSET 1
	mov	WORD PTR -16[bp],ax
	mov	WORD PTR -14[bp],dx
	_imul3 	ax,cx,OFFSET 28
	add	bx,ax
	mov	es,si
	mov	ax,WORD PTR es:20[bx]
	mov	dx,WORD PTR es:22[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	cx,WORD PTR -6[bp]
	add	cx,OFFSET 1
	mov	WORD PTR -20[bp],ax
	mov	WORD PTR -18[bp],dx
	_imul3 	ax,cx,OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -20[bp]
	mov	dx,WORD PTR -18[bp]
	mov	es,si
	add	ax,WORD PTR es:24[bx]
	adc	dx,WORD PTR es:26[bx]
	mov	cx,WORD PTR -16[bp]
	mov	bx,WORD PTR -14[bp]
	cmp	bx,dx
	ja	L01948
	jb	L02350
	cmp	cx,ax
	ja	L01948
L02350:
; Line 698
; Line 699
	mov	WORD PTR -8[bp],OFFSET 1
; Line 700
	mov	ax,OFFSET 1
	jmp	L01940
; Line 701
; Line 702
L01948:
; Line 702
	jmp	L01945
L01946:
; Line 703
	mov	WORD PTR -6[bp],OFFSET 0
	jmp	L01949
L01950:
	add	WORD PTR -6[bp],OFFSET 1
L01949:
	mov	ax,WORD PTR -6[bp]
	les	bx,WORD PTR 6[bp]
	cmp	ax,WORD PTR es:2[bx]
	jae	L01951
; Line 704
; Line 705
; Line 706
; Line 707
; Line 708
	mov	ax,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	cx,WORD PTR 8[bp]
	add	bx,ax
	_imul3 	ax,WORD PTR -6[bp],OFFSET 28
	add	bx,ax
	mov	es,cx
	mov	ax,WORD PTR es:20[bx]
	mov	dx,WORD PTR es:22[bx]
	les	bx,WORD PTR 10[bp]
	cmp	WORD PTR es:10[bx],dx
	jb	L01955
	ja	L02351
	cmp	WORD PTR es:8[bx],ax
	jb	L01955
L02351:
	mov	ax,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	cx,WORD PTR 8[bp]
	add	bx,ax
	_imul3 	ax,WORD PTR -6[bp],OFFSET 28
	add	bx,ax
	mov	es,cx
	mov	ax,WORD PTR es:20[bx]
	mov	dx,WORD PTR es:22[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	WORD PTR -12[bp],ax
	mov	WORD PTR -10[bp],dx
	_imul3 	ax,WORD PTR -6[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -12[bp]
	mov	dx,WORD PTR -10[bp]
	mov	es,si
	add	ax,WORD PTR es:24[bx]
	adc	dx,WORD PTR es:26[bx]
	les	bx,WORD PTR 10[bp]
	cmp	WORD PTR es:10[bx],dx
	jb	L01954
	ja	L02352
	cmp	WORD PTR es:8[bx],ax
	jbe	L01954
L02352:
L01955:
	les	bx,WORD PTR 10[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 10[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	WORD PTR -12[bp],ax
	mov	WORD PTR -10[bp],dx
	_imul3 	ax,WORD PTR -6[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -12[bp]
	mov	dx,WORD PTR -10[bp]
	mov	es,si
	cmp	dx,WORD PTR es:22[bx]
	jb	L01953
	ja	L02353
	cmp	ax,WORD PTR es:20[bx]
	jb	L01953
L02353:
	les	bx,WORD PTR 10[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 10[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	WORD PTR -12[bp],ax
	mov	WORD PTR -10[bp],dx
	_imul3 	ax,WORD PTR -6[bp],OFFSET 28
	add	bx,ax
	mov	es,si
	mov	ax,WORD PTR es:20[bx]
	mov	dx,WORD PTR es:22[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	WORD PTR -16[bp],ax
	mov	WORD PTR -14[bp],dx
	_imul3 	ax,WORD PTR -6[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -16[bp]
	mov	dx,WORD PTR -14[bp]
	mov	es,si
	add	ax,WORD PTR es:24[bx]
	adc	dx,WORD PTR es:26[bx]
	mov	cx,WORD PTR -12[bp]
	mov	bx,WORD PTR -10[bp]
	cmp	bx,dx
	ja	L01953
	jb	L02354
	cmp	cx,ax
	ja	L01953
L02354:
L01954:
; Line 709
; Line 710
	mov	WORD PTR -8[bp],OFFSET 1
; Line 711
	jmp	L01951
; Line 712
; Line 713
L01953:
; Line 713
	jmp	L01950
L01951:
; Line 714
	mov	ax,WORD PTR -8[bp]
	jmp	L01940
; Line 715
; Line 715
L01940:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 718
; peParEntry = 0006
; btHardDisk = 000a
; dwFlag = 000c
; blIsFormat = 0010
; pLabel = 0012
; hWnd = 0016
; nCreateErrorCode = 001a
_DoCreatePartition:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02355
	call	FAR PTR __aFchkstk
	push	si
	push	di
; nPartition = fffc
; PartitionLogicalMBR = fdfc
; i = fdfa
; PartitionMBR = fbfa
; dwStartSec = fbf6
; nResizeErrCode = fbf4
; ParEntryTemp1 = fbe4
; DriveParam = fbca
; dwTempStart = fbc6
; nExtend = fbc4
; nDeleteErrCode = fbc2
; dwTemp = fbbe
; ParHard = f8a2
; Line 719
; Line 720
; Line 721
; Line 722
	mov	WORD PTR -1034[bp],OFFSET 0
	mov	WORD PTR -1032[bp],OFFSET 0
; Line 723
	mov	WORD PTR -1082[bp],OFFSET 0
	mov	WORD PTR -1080[bp],OFFSET 0
; Line 724
; Line 725
; Line 726
; Line 727
; Line 728
	mov	WORD PTR -1090[bp],OFFSET 0
	mov	WORD PTR -1088[bp],OFFSET 0
; Line 729
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 0
; Line 730
	mov	WORD PTR -1036[bp],OFFSET 0
; Line 731
	mov	WORD PTR -1086[bp],OFFSET 0
; Line 732
	les	bx,WORD PTR 6[bp]
	mov	al,BYTE PTR es:[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01977
	les	bx,WORD PTR 6[bp]
	mov	al,BYTE PTR es:[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 128
	je	L01977
; Line 733
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:[bx],OFFSET 0
; Line 734
L01977:
	push	OFFSET 796
	push	OFFSET 0
	lea	ax,WORD PTR -1886[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 735
	lea	ax,WORD PTR -1078[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetDriveParam
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L01979
; Line 736
; Line 737
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 102
; Line 738
	mov	ax,OFFSET 0
	jmp	L01963
; Line 739
; Line 740
L01979:
	lea	ax,WORD PTR -1078[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -1030[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01981
; Line 741
; Line 742
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 103
; Line 743
	mov	ax,OFFSET 0
	jmp	L01963
; Line 744
; Line 745
L01981:
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 128
	jle	L01982
	mov	ax,WORD PTR -1076[bp]
	and	ax,OFFSET 4
	cmp	ax,OFFSET 0
	je	L01982
; Line 746
; Line 747
	mov	WORD PTR -518[bp],OFFSET 0
	jmp	L01986
L01983:
; Line 748
; Line 749
	mov	ax,WORD PTR -518[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L01987
; Line 750
	jmp	L01985
; Line 751
L01987:
L01984:
; Line 751
	add	WORD PTR -518[bp],OFFSET 1
L01986:
	cmp	WORD PTR -518[bp],OFFSET 4
	jl	L01983
L01985:
; Line 752
	les	bx,WORD PTR 6[bp]
	cmp	WORD PTR es:8[bx],OFFSET 0
	jne	L02356
	cmp	WORD PTR es:10[bx],OFFSET 0
	je	L01988
L02356:
	cmp	WORD PTR -518[bp],OFFSET 4
	jne	L01988
; Line 753
; Line 754
	lea	ax,WORD PTR -1078[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	mov	ax,OFFSET StdMBR
	mov	dx,ds
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01990
; Line 755
; Line 756
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 757
	mov	ax,OFFSET 0
	jmp	L01963
; Line 758
; Line 759
L01990:
; Line 760
L01988:
; Line 761
L01982:
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 128
	jle	L01991
; Line 762
; Line 763
	mov	al,BYTE PTR -1030[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	jne	L01992
; Line 764
; Line 765
	lea	ax,WORD PTR -1078[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	mov	ax,OFFSET StdMBR
	mov	dx,ds
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L01994
; Line 766
; Line 767
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 768
	mov	ax,OFFSET 0
	jmp	L01963
; Line 769
; Line 770
L01994:
; Line 771
L01992:
; Line 773
L01991:
	push	WORD PTR -1064[bp]
	push	WORD PTR -1066[bp]
	push	WORD PTR -1068[bp]
	push	WORD PTR -1070[bp]
	call	FAR PTR __aFulmul
	les	bx,WORD PTR 6[bp]
	cmp	WORD PTR es:14[bx],dx
	ja	L01995
	jb	L02357
	cmp	WORD PTR es:12[bx],ax
	jae	L01995
L02357:
; Line 774
; Line 776
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 105
; Line 777
	mov	ax,OFFSET 0
	jmp	L01963
; Line 778
; Line 780
L01995:
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	cmp	dx,WORD PTR -1060[bp]
	jb	L01996
	ja	L02358
	cmp	ax,WORD PTR -1062[bp]
	jbe	L01996
L02358:
; Line 781
; Line 782
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 105
; Line 783
	mov	ax,OFFSET 0
	jmp	L01963
; Line 784
; Line 785
L01996:
	push	WORD PTR 14[bp]
	push	WORD PTR 12[bp]
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	sub	sp,OFFSET 26
	lea	si,WORD PTR -1078[bp]
	mov	di,sp
	push	ss
	pop	es
	mov	cx,OFFSET 13
	rep
	movs
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	call	FAR PTR _CaculateStartAndEnd
	add	sp,OFFSET 36
; Line 787
	les	bx,WORD PTR 6[bp]
	cmp	WORD PTR es:10[bx],OFFSET 0
	ja	L01997
	jb	L02359
	cmp	WORD PTR es:8[bx],OFFSET 63
	ja	L01997
L02359:
	cmp	WORD PTR 12[bp],OFFSET 3
	jne	L01997
	cmp	WORD PTR 14[bp],OFFSET 0
	jne	L01997
; Line 788
; Line 789
	push	WORD PTR -1068[bp]
	push	WORD PTR -1070[bp]
	push	WORD PTR -1064[bp]
	push	WORD PTR -1066[bp]
	call	FAR PTR __aFulmul
	les	bx,WORD PTR 6[bp]
	mov	WORD PTR es:8[bx],ax
	mov	WORD PTR es:10[bx],dx
; Line 790
	push	WORD PTR -1068[bp]
	push	WORD PTR -1070[bp]
	push	WORD PTR -1064[bp]
	push	WORD PTR -1066[bp]
	call	FAR PTR __aFulmul
	les	bx,WORD PTR 6[bp]
	mov	cx,WORD PTR es:12[bx]
	mov	bx,WORD PTR es:14[bx]
	sub	cx,ax
	sbb	bx,dx
	mov	WORD PTR -1890[bp],cx
	mov	WORD PTR -1888[bp],bx
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR -1890[bp]
	mov	dx,WORD PTR -1888[bp]
	mov	WORD PTR es:12[bx],ax
	mov	WORD PTR es:14[bx],dx
; Line 791
	push	WORD PTR 14[bp]
	push	WORD PTR 12[bp]
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	sub	sp,OFFSET 26
	lea	si,WORD PTR -1078[bp]
	mov	di,sp
	push	ss
	pop	es
	mov	cx,OFFSET 13
	rep
	movs
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	call	FAR PTR _CaculateStartAndEnd
	add	sp,OFFSET 36
; Line 792
; Line 793
L01997:
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR -1066[bp]
	mov	dx,WORD PTR -1064[bp]
	cmp	WORD PTR es:10[bx],dx
	ja	L01998
	jb	L02360
	cmp	WORD PTR es:8[bx],ax
	ja	L01998
L02360:
	cmp	WORD PTR 12[bp],OFFSET 3
	jne	L01998
	cmp	WORD PTR 14[bp],OFFSET 0
	jne	L01998
; Line 794
; Line 795
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 107
; Line 796
	mov	ax,OFFSET 0
	jmp	L01963
; Line 797
; Line 798
L01998:
	lea	ax,WORD PTR -1886[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetPartitionInfoEx
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L01999
; Line 799
; Line 800
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 106
; Line 801
	mov	ax,OFFSET 0
	jmp	L01963
; Line 802
; Line 804
L01999:
	cmp	WORD PTR 12[bp],OFFSET 1
	jne	L02361
	cmp	WORD PTR 14[bp],OFFSET 0
	je	L02001
L02361:
	cmp	WORD PTR 12[bp],OFFSET 2
	jne	L02000
	cmp	WORD PTR 14[bp],OFFSET 0
	jne	L02000
L02001:
; Line 805
; Line 806
	mov	WORD PTR -4[bp],OFFSET 0
	jmp	L02005
L02002:
; Line 807
; Line 808
	mov	ax,WORD PTR -4[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	jne	L02006
; Line 809
	jmp	L02004
; Line 810
L02006:
L02003:
; Line 810
	add	WORD PTR -4[bp],OFFSET 1
L02005:
	cmp	WORD PTR -4[bp],OFFSET 4
	jl	L02002
L02004:
; Line 811
	cmp	WORD PTR -4[bp],OFFSET 4
	jne	L02007
; Line 812
; Line 813
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 108
; Line 814
	mov	ax,OFFSET 0
	jmp	L01963
; Line 815
; Line 816
L02007:
	mov	WORD PTR -1084[bp],OFFSET 0
	jmp	L02011
L02008:
; Line 817
; Line 818
; Line 819
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 5
	je	L02013
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 15
	jne	L02012
L02013:
; Line 820
	jmp	L02010
; Line 821
L02012:
L02009:
; Line 821
	add	WORD PTR -1084[bp],OFFSET 1
L02011:
	cmp	WORD PTR -1084[bp],OFFSET 4
	jl	L02008
L02010:
; Line 823
	mov	WORD PTR -518[bp],OFFSET 0
	jmp	L02017
L02014:
; Line 824
; Line 825
	mov	ax,WORD PTR -518[bp]
	cmp	WORD PTR -1084[bp],ax
	je	L02018
	mov	ax,WORD PTR -518[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L02018
; Line 826
; Line 827
; Line 828
	mov	ax,WORD PTR -518[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	les	bx,WORD PTR 6[bp]
	cmp	WORD PTR es:10[bx],dx
	jb	L02019
	ja	L02362
	cmp	WORD PTR es:8[bx],ax
	jb	L02019
L02362:
	mov	ax,WORD PTR -518[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	bx,WORD PTR -518[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -584[bp]
	add	si,bx
	add	ax,WORD PTR ss:12[si]
	adc	dx,WORD PTR ss:14[si]
	les	bx,WORD PTR 6[bp]
	cmp	WORD PTR es:10[bx],dx
	ja	L02019
	jb	L02363
	cmp	WORD PTR es:8[bx],ax
	jae	L02019
L02363:
; Line 829
; Line 830
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 109
; Line 831
	mov	ax,OFFSET 0
	jmp	L01963
; Line 832
; Line 833
L02019:
; Line 834
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	bx,WORD PTR -518[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -584[bp]
	add	si,bx
	cmp	dx,WORD PTR ss:10[si]
	jb	L02020
	ja	L02364
	cmp	ax,WORD PTR ss:8[si]
	jbe	L02020
L02364:
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	bx,WORD PTR -518[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -584[bp]
	add	si,bx
	mov	cx,WORD PTR ss:8[si]
	mov	bx,WORD PTR ss:10[si]
	mov	si,WORD PTR -518[bp]
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	lea	di,WORD PTR -584[bp]
	add	di,si
	add	cx,WORD PTR ss:12[di]
	adc	bx,WORD PTR ss:14[di]
	cmp	dx,bx
	ja	L02020
	jb	L02365
	cmp	ax,cx
	jae	L02020
L02365:
; Line 835
; Line 836
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 109
; Line 837
	mov	ax,OFFSET 0
	jmp	L01963
; Line 838
; Line 839
L02020:
; Line 840
L02018:
L02015:
; Line 840
	add	WORD PTR -518[bp],OFFSET 1
L02017:
	cmp	WORD PTR -518[bp],OFFSET 4
	jl	L02014
L02016:
; Line 842
; Line 843
	cmp	WORD PTR 12[bp],OFFSET 2
	jne	L02366
	cmp	WORD PTR 14[bp],OFFSET 0
	je	L02021
L02366:
	cmp	WORD PTR -1084[bp],OFFSET 4
	je	L02021
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	les	bx,WORD PTR 6[bp]
	cmp	WORD PTR es:10[bx],dx
	jb	L02021
	ja	L02367
	cmp	WORD PTR es:8[bx],ax
	jb	L02021
L02367:
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	bx,WORD PTR -1084[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -584[bp]
	add	si,bx
	mov	cx,WORD PTR ss:8[si]
	mov	bx,WORD PTR ss:10[si]
	mov	si,WORD PTR -1084[bp]
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	lea	di,WORD PTR -584[bp]
	add	di,si
	add	cx,WORD PTR ss:12[di]
	adc	bx,WORD PTR ss:14[di]
	cmp	dx,bx
	ja	L02021
	jb	L02368
	cmp	ax,cx
	ja	L02021
L02368:
; Line 844
; Line 845
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	lea	ax,WORD PTR -1886[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _VerifyPrimaryPartition
	add	sp,OFFSET 8
	cmp	ax,OFFSET 0
	je	L02022
; Line 846
; Line 847
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 109
; Line 848
	mov	ax,OFFSET 0
	jmp	L01963
; Line 849
; Line 850
L02022:
; Line 851
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	les	bx,WORD PTR 6[bp]
	cmp	WORD PTR es:8[bx],ax
	jne	L02023
	cmp	WORD PTR es:10[bx],dx
	jne	L02023
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	bx,WORD PTR -1084[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -584[bp]
	add	si,bx
	mov	cx,WORD PTR ss:8[si]
	mov	bx,WORD PTR ss:10[si]
	mov	si,WORD PTR -1084[bp]
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	lea	di,WORD PTR -584[bp]
	add	di,si
	add	cx,WORD PTR ss:12[di]
	adc	bx,WORD PTR ss:14[di]
	cmp	ax,cx
	jne	L02023
	cmp	dx,bx
	jne	L02023
; Line 852
; cpf = f89e
; Line 853
	mov	WORD PTR -1890[bp],OFFSET 0
	mov	WORD PTR -1888[bp],OFFSET 0
; Line 854
	lea	ax,WORD PTR -1086[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	WORD PTR -1888[bp]
	push	WORD PTR -1890[bp]
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	les	bx,WORD PTR 6[bp]
	push	WORD PTR es:10[bx]
	push	WORD PTR es:8[bx]
	call	FAR PTR _DoDeletePartition
	add	sp,OFFSET 14
; Line 855
; Line 856
	jmp	L02025
L02023:
; Line 856
; Line 857
; Line 858
	push	OFFSET 16
	push	OFFSET 0
	lea	ax,WORD PTR -1052[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 859
	cmp	WORD PTR -1884[bp],OFFSET 0
	je	L02027
; Line 860
; Line 861
; Line 862
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	les	bx,WORD PTR 6[bp]
	cmp	WORD PTR es:10[bx],dx
	jb	L02028
	ja	L02369
	cmp	WORD PTR es:8[bx],ax
	jb	L02028
L02369:
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	cmp	dx,WORD PTR -1796[bp]
	ja	L02028
	jb	L02370
	cmp	ax,WORD PTR -1798[bp]
	ja	L02028
L02370:
; Line 863
; Line 864
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	WORD PTR -1044[bp],ax
	mov	WORD PTR -1042[bp],dx
; Line 865
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	bx,WORD PTR -1084[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -584[bp]
	add	si,bx
	add	ax,WORD PTR ss:12[si]
	adc	dx,WORD PTR ss:14[si]
	sub	ax,WORD PTR -1044[bp]
	sbb	dx,WORD PTR -1042[bp]
	mov	WORD PTR -1040[bp],ax
	mov	WORD PTR -1038[bp],dx
; Line 866
; Line 867
L02028:
	mov	ax,WORD PTR -1884[bp]
	add	ax,OFFSET -1
	mov	cx,OFFSET 28
	imul	cx
	lea	bx,WORD PTR -1818[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:20[bx]
	mov	dx,WORD PTR ss:22[bx]
	mov	cx,WORD PTR -1884[bp]
	add	cx,OFFSET -1
	mov	WORD PTR -1890[bp],ax
	mov	WORD PTR -1888[bp],dx
	mov	ax,cx
	mov	cx,OFFSET 28
	imul	cx
	lea	bx,WORD PTR -1818[bp]
	add	bx,ax
	mov	ax,WORD PTR -1890[bp]
	mov	dx,WORD PTR -1888[bp]
	add	ax,WORD PTR ss:24[bx]
	adc	dx,WORD PTR ss:26[bx]
	les	bx,WORD PTR 6[bp]
	cmp	WORD PTR es:10[bx],dx
	jb	L02029
	ja	L02371
	cmp	WORD PTR es:8[bx],ax
	jb	L02029
L02371:
; Line 868
; Line 869
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	WORD PTR -1044[bp],ax
	mov	WORD PTR -1042[bp],dx
; Line 870
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	sub	ax,WORD PTR -1044[bp]
	sbb	dx,WORD PTR -1042[bp]
	mov	WORD PTR -1040[bp],ax
	mov	WORD PTR -1038[bp],dx
; Line 871
; Line 872
L02029:
; Line 873
	jmp	L02030
L02027:
; Line 873
; Line 874
; Line 875
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	WORD PTR -1044[bp],ax
	mov	WORD PTR -1042[bp],dx
; Line 876
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -584[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	bx,WORD PTR -1084[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -584[bp]
	add	si,bx
	add	ax,WORD PTR ss:12[si]
	adc	dx,WORD PTR ss:14[si]
	sub	ax,WORD PTR -1044[bp]
	sbb	dx,WORD PTR -1042[bp]
	mov	WORD PTR -1040[bp],ax
	mov	WORD PTR -1038[bp],dx
; Line 877
L02030:
; Line 878
	lea	ax,WORD PTR -1036[bp]
	mov	dx,ss
	push	dx
	push	ax
	lea	ax,WORD PTR -1052[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _DoResizeExtendPartition
	add	sp,OFFSET 10
	cmp	ax,OFFSET 0
	jne	L02031
; Line 879
; Line 880
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 112
; Line 881
	mov	ax,OFFSET 0
	jmp	L01963
; Line 882
; Line 883
L02031:
L02025:
; Line 884
	lea	ax,WORD PTR -1078[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -1030[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L02033
; Line 885
; Line 886
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 103
; Line 887
	mov	ax,OFFSET 0
	jmp	L01963
; Line 888
; Line 889
L02033:
; Line 890
L02021:
	push	OFFSET 16
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	mov	ax,WORD PTR -4[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	cx,WORD PTR -584[bp]
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 891
	lea	ax,WORD PTR -1078[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -1030[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L02036
; Line 892
; Line 893
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 894
	mov	ax,OFFSET 0
	jmp	L01963
; Line 895
; Line 896
L02036:
; Line 897
L02000:
	cmp	WORD PTR 12[bp],OFFSET 3
	jne	L02037
	cmp	WORD PTR 14[bp],OFFSET 0
	jne	L02037
; Line 898
; Line 899
	mov	WORD PTR -1034[bp],OFFSET 0
	mov	WORD PTR -1032[bp],OFFSET 0
; Line 900
	mov	WORD PTR -1084[bp],OFFSET 0
	jmp	L02038
L02039:
	add	WORD PTR -1084[bp],OFFSET 1
L02038:
	mov	ax,WORD PTR -1084[bp]
	cmp	ax,WORD PTR -1886[bp]
	jae	L02040
; Line 901
; Line 902
; Line 903
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1882[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 5
	je	L02043
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1882[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 15
	jne	L02042
L02043:
; Line 904
; bNeedResize = f8a0
; Line 905
	mov	WORD PTR -1888[bp],OFFSET 0
; Line 906
	push	OFFSET 16
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	cx,WORD PTR -1882[bp]
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	lea	ax,WORD PTR -1052[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 907
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	mov	bx,WORD PTR -1084[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -1882[bp]
	add	si,bx
	cmp	WORD PTR ss:10[si],dx
	jb	L02046
	ja	L02372
	cmp	WORD PTR ss:8[si],ax
	jbe	L02046
L02372:
; Line 908
; Line 909
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	mov	WORD PTR -1044[bp],ax
	mov	WORD PTR -1042[bp],dx
; Line 910
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1882[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	add	WORD PTR -1040[bp],ax
	adc	WORD PTR -1038[bp],dx
; Line 911
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	sub	WORD PTR -1040[bp],ax
	sbb	WORD PTR -1038[bp],dx
; Line 912
	mov	WORD PTR -1888[bp],OFFSET 1
; Line 913
; Line 914
L02046:
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	cx,WORD PTR -1044[bp]
	mov	bx,WORD PTR -1042[bp]
	add	cx,WORD PTR -1040[bp]
	adc	bx,WORD PTR -1038[bp]
	cmp	dx,bx
	jb	L02047
	ja	L02373
	cmp	ax,cx
	jbe	L02047
L02373:
; Line 915
; Line 916
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	sub	ax,WORD PTR -1044[bp]
	sbb	dx,WORD PTR -1042[bp]
	mov	WORD PTR -1040[bp],ax
	mov	WORD PTR -1038[bp],dx
; Line 917
	mov	WORD PTR -1888[bp],OFFSET 1
; Line 918
; Line 919
L02047:
	cmp	WORD PTR -1888[bp],OFFSET 0
	je	L02048
; Line 920
; Line 921
	lea	ax,WORD PTR -1036[bp]
	mov	dx,ss
	push	dx
	push	ax
	lea	ax,WORD PTR -1052[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _DoResizeExtendPartition
	add	sp,OFFSET 10
	cmp	ax,OFFSET 0
	jne	L02049
; Line 922
; Line 923
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 112
; Line 924
	mov	ax,OFFSET 0
	jmp	L01963
; Line 925
; Line 926
L02049:
	lea	ax,WORD PTR -1886[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetPartitionInfoEx
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L02050
; Line 927
; Line 928
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 106
; Line 929
	mov	ax,OFFSET 0
	jmp	L01963
; Line 930
; Line 931
L02050:
; Line 932
L02048:
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1882[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	WORD PTR -1034[bp],ax
	mov	WORD PTR -1032[bp],dx
; Line 933
	jmp	L02040
; Line 934
; Line 935
L02042:
; Line 935
	jmp	L02039
L02040:
; Line 936
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	lea	ax,WORD PTR -1886[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _VerifyLogicalPartition
	add	sp,OFFSET 8
	cmp	ax,OFFSET 0
	je	L02051
; Line 937
; Line 938
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 109
; Line 939
	mov	ax,OFFSET 0
	jmp	L01963
; Line 940
; Line 942
L02051:
	cmp	WORD PTR -1034[bp],OFFSET 0
	jne	L02052
	cmp	WORD PTR -1032[bp],OFFSET 0
	jne	L02052
; Line 943
; Line 944
	push	OFFSET 16
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	lea	ax,WORD PTR -1052[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 945
	mov	BYTE PTR -1048[bp],OFFSET 5
; Line 946
	push	OFFSET 0
	push	OFFSET 2
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	sub	sp,OFFSET 26
	lea	si,WORD PTR -1078[bp]
	mov	di,sp
	push	ss
	pop	es
	mov	cx,OFFSET 13
	rep
	movs
	lea	ax,WORD PTR -1052[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _CaculateStartAndEnd
	add	sp,OFFSET 36
; Line 947
	lea	ax,WORD PTR -1078[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -1052[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _DoCreatePriPar
	add	sp,OFFSET 10
	cmp	ax,OFFSET 0
	jne	L02054
; Line 948
; Line 949
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 114
; Line 950
	mov	ax,OFFSET 0
	jmp	L01963
; Line 951
; Line 952
L02054:
	lea	ax,WORD PTR -1886[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetPartitionInfoEx
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L02055
; Line 953
; Line 954
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 106
; Line 955
	mov	ax,OFFSET 0
	jmp	L01963
; Line 956
; Line 957
L02055:
	mov	WORD PTR -1084[bp],OFFSET 0
	jmp	L02056
L02057:
	add	WORD PTR -1084[bp],OFFSET 1
L02056:
	mov	ax,WORD PTR -1084[bp]
	cmp	ax,WORD PTR -1886[bp]
	jae	L02058
; Line 958
; Line 959
; Line 960
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1882[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 5
	je	L02061
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1882[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 15
	jne	L02060
L02061:
; Line 961
; Line 962
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1882[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	WORD PTR -1034[bp],ax
	mov	WORD PTR -1032[bp],dx
; Line 963
	jmp	L02058
; Line 964
; Line 965
L02060:
; Line 965
	jmp	L02057
L02058:
; Line 966
; Line 968
L02052:
; Line 969
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	bx,WORD PTR -1084[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -1882[bp]
	add	si,bx
	mov	cx,WORD PTR ss:8[si]
	mov	bx,WORD PTR ss:10[si]
	mov	si,WORD PTR -1084[bp]
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	lea	di,WORD PTR -1882[bp]
	add	di,si
	add	cx,WORD PTR ss:12[di]
	adc	bx,WORD PTR ss:14[di]
	cmp	dx,bx
	ja	L02062
	jb	L02374
	cmp	ax,cx
	ja	L02062
L02374:
	mov	ax,WORD PTR -1084[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1882[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	les	bx,WORD PTR 6[bp]
	cmp	WORD PTR es:10[bx],dx
	jb	L02062
	ja	L02375
	cmp	WORD PTR es:8[bx],ax
	jb	L02062
L02375:
; Line 970
; Line 971
	mov	ax,WORD PTR -1034[bp]
	mov	dx,WORD PTR -1032[bp]
	mov	WORD PTR -1090[bp],ax
	mov	WORD PTR -1088[bp],dx
; Line 972
	mov	WORD PTR -518[bp],OFFSET 0
	jmp	L02063
L02064:
	add	WORD PTR -518[bp],OFFSET 1
L02063:
	mov	ax,WORD PTR -518[bp]
	cmp	ax,WORD PTR -1884[bp]
	jae	L02065
; Line 973
; Line 974
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	mov	WORD PTR -1890[bp],ax
	mov	WORD PTR -1888[bp],dx
	_imul3 	ax,WORD PTR -518[bp],OFFSET 28
	lea	bx,WORD PTR -1818[bp]
	add	bx,ax
	mov	ax,WORD PTR -1890[bp]
	mov	dx,WORD PTR -1888[bp]
	cmp	WORD PTR ss:22[bx],dx
	jb	L02067
	ja	L02376
	cmp	WORD PTR ss:20[bx],ax
	jbe	L02067
L02376:
; Line 975
	jmp	L02065
; Line 976
L02067:
	_imul3 	ax,WORD PTR -518[bp],OFFSET 28
	lea	bx,WORD PTR -1818[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:20[bx]
	mov	dx,WORD PTR ss:22[bx]
	mov	WORD PTR -1090[bp],ax
	mov	WORD PTR -1088[bp],dx
; Line 977
; Line 977
	jmp	L02064
L02065:
; Line 979
	push	OFFSET 512
	push	OFFSET 0
	lea	ax,WORD PTR -516[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 980
	mov	WORD PTR -6[bp],OFFSET -21931
; Line 981
	push	OFFSET 16
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	lea	ax,WORD PTR -70[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 982
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	mov	WORD PTR -1082[bp],ax
	mov	WORD PTR -1080[bp],dx
; Line 983
	mov	ax,WORD PTR -1066[bp]
	mov	dx,WORD PTR -1064[bp]
	mov	WORD PTR -62[bp],ax
	mov	WORD PTR -60[bp],dx
; Line 984
	mov	BYTE PTR -69[bp],OFFSET 1
; Line 985
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:12[bx]
	mov	dx,WORD PTR es:14[bx]
	sub	ax,WORD PTR -1066[bp]
	sbb	dx,WORD PTR -1064[bp]
	mov	WORD PTR -58[bp],ax
	mov	WORD PTR -56[bp],dx
; Line 986
	mov	ax,WORD PTR -518[bp]
	cmp	ax,WORD PTR -1884[bp]
	je	L02069
; Line 987
; Line 988
	push	OFFSET 16
	_imul3 	ax,WORD PTR -518[bp],OFFSET 28
	lea	cx,WORD PTR -1818[bp]
	add	cx,ax
	mov	ax,OFFSET 12
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	lea	ax,WORD PTR -54[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 989
	mov	BYTE PTR -53[bp],OFFSET 0
; Line 990
	mov	BYTE PTR -50[bp],OFFSET 5
; Line 991
	_imul3 	ax,WORD PTR -518[bp],OFFSET 28
	lea	bx,WORD PTR -1818[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	add	WORD PTR -42[bp],ax
	adc	WORD PTR -40[bp],dx
; Line 992
	mov	ax,WORD PTR -1034[bp]
	mov	dx,WORD PTR -1032[bp]
	sub	WORD PTR -46[bp],ax
	sbb	WORD PTR -44[bp],dx
; Line 993
; Line 994
L02069:
	lea	ax,WORD PTR -1078[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -516[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR -1080[bp]
	push	WORD PTR -1082[bp]
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L02072
; Line 995
; Line 996
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 997
	mov	ax,OFFSET 0
	jmp	L01963
; Line 998
; Line 1000
L02072:
	mov	ax,WORD PTR -1082[bp]
	mov	dx,WORD PTR -1080[bp]
	cmp	WORD PTR -1090[bp],ax
	jne	L02377
	cmp	WORD PTR -1088[bp],dx
	je	L02073
L02377:
; Line 1001
; Line 1002
	lea	ax,WORD PTR -1078[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -516[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR -1088[bp]
	push	WORD PTR -1090[bp]
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L02075
; Line 1003
; Line 1004
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 103
; Line 1005
	mov	ax,OFFSET 0
	jmp	L01963
; Line 1006
; Line 1007
L02075:
	lea	ax,WORD PTR -1078[bp]
	mov	dx,ss
	push	dx
	push	ax
	lea	ax,WORD PTR -516[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	WORD PTR -1032[bp]
	push	WORD PTR -1034[bp]
	call	FAR PTR ?IsValidEMBR@@ZAHKPEUPARTITION_SEC@@PEUBIOS_DRIVE_PARAM@@@Z
	add	sp,OFFSET 12
	cmp	ax,OFFSET 0
	jne	L02076
; Line 1008
	push	OFFSET 512
	push	OFFSET 0
	lea	ax,WORD PTR -516[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 1009
L02076:
	push	OFFSET 16
	lea	ax,WORD PTR -70[bp]
	mov	dx,ss
	push	dx
	push	ax
	lea	ax,WORD PTR -1052[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 1010
	push	OFFSET 512
	push	OFFSET 0
	lea	ax,WORD PTR -516[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 1011
	mov	WORD PTR -6[bp],OFFSET -21931
; Line 1013
	push	OFFSET 16
	lea	ax,WORD PTR -1052[bp]
	mov	dx,ss
	push	dx
	push	ax
	lea	ax,WORD PTR -70[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 1014
	push	OFFSET 16
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	lea	ax,WORD PTR -54[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 1015
	mov	BYTE PTR -53[bp],OFFSET 0
; Line 1016
	mov	BYTE PTR -50[bp],OFFSET 5
; Line 1017
	mov	ax,WORD PTR -1034[bp]
	mov	dx,WORD PTR -1032[bp]
	sub	WORD PTR -46[bp],ax
	sbb	WORD PTR -44[bp],dx
; Line 1018
	lea	ax,WORD PTR -1078[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -516[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR -1088[bp]
	push	WORD PTR -1090[bp]
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L02081
; Line 1019
; Line 1020
	les	bx,WORD PTR 26[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 1021
	mov	ax,OFFSET 0
	jmp	L01963
; Line 1022
; Line 1023
L02081:
; Line 1024
L02073:
	mov	ax,OFFSET 1
	jmp	L01963
; Line 1025
; Line 1026
L02062:
; Line 1027
L02037:
	mov	ax,OFFSET 1
	jmp	L01963
; Line 1028
; Line 1028
L01963:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 1031
; pEntry = 0006
; DriveParam = 000a
; btHardDisk = 0024
; dwFlag = 0026
_CaculateStartAndEnd:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02378
	call	FAR PTR __aFchkstk
	push	si
	push	di
; btSector = fffc
; dwCylinder = fff8
; btHead = fff6
; dwTemp = fff2
; Line 1032
	mov	WORD PTR -14[bp],OFFSET 0
	mov	WORD PTR -12[bp],OFFSET 0
; Line 1033
	mov	BYTE PTR -10[bp],OFFSET 0
; Line 1034
	mov	BYTE PTR -4[bp],OFFSET 0
; Line 1035
	mov	WORD PTR -8[bp],OFFSET 0
	mov	WORD PTR -6[bp],OFFSET 0
; Line 1036
	push	OFFSET 0
	mov	al,BYTE PTR 36[bp]
	and	ax,OFFSET 255
	push	ax
	les	bx,WORD PTR 6[bp]
	push	WORD PTR es:10[bx]
	push	WORD PTR es:8[bx]
	call	FAR PTR _LocatePartition
	add	sp,OFFSET 8
	mov	WORD PTR -14[bp],ax
	mov	WORD PTR -12[bp],dx
; Line 1037
	cmp	WORD PTR -14[bp],OFFSET -1
	jne	L02091
	cmp	WORD PTR -12[bp],OFFSET -1
	jne	L02091
; Line 1038
	mov	ax,OFFSET 0
	jmp	L02086
; Line 1039
	jmp	L02092
L02091:
; Line 1039
; Line 1040
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR -14[bp]
	mov	dx,WORD PTR -12[bp]
	mov	WORD PTR es:8[bx],ax
	mov	WORD PTR es:10[bx],dx
L02092:
; Line 1041
	push	OFFSET 1
	mov	al,BYTE PTR 36[bp]
	and	ax,OFFSET 255
	push	ax
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 6[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	push	dx
	push	ax
	call	FAR PTR _LocatePartition
	add	sp,OFFSET 8
	add	ax,OFFSET -1
	adc	dx,OFFSET -1
	mov	WORD PTR -14[bp],ax
	mov	WORD PTR -12[bp],dx
; Line 1042
	cmp	WORD PTR -14[bp],OFFSET -1
	jne	L02093
	cmp	WORD PTR -12[bp],OFFSET -1
	jne	L02093
; Line 1043
	mov	ax,OFFSET 0
	jmp	L02086
; Line 1044
L02093:
	les	bx,WORD PTR 6[bp]
	mov	ax,WORD PTR -14[bp]
	mov	dx,WORD PTR -12[bp]
	sub	ax,WORD PTR es:8[bx]
	sbb	dx,WORD PTR es:10[bx]
	add	ax,OFFSET 1
	adc	dx,OFFSET 0
	les	bx,WORD PTR 6[bp]
	mov	WORD PTR es:12[bx],ax
	mov	WORD PTR es:14[bx],dx
; Line 1045
	push	WORD PTR 20[bp]
	push	WORD PTR 18[bp]
	push	WORD PTR 24[bp]
	push	WORD PTR 22[bp]
	les	bx,WORD PTR 6[bp]
	push	WORD PTR es:10[bx]
	push	WORD PTR es:8[bx]
	call	FAR PTR __aFuldiv
	push	dx
	push	ax
	call	FAR PTR __aFulrem
	and	ax,OFFSET 255
	and	dx,OFFSET 0
	mov	BYTE PTR -10[bp],al
; Line 1046
	push	WORD PTR 24[bp]
	push	WORD PTR 22[bp]
	les	bx,WORD PTR 6[bp]
	push	WORD PTR es:10[bx]
	push	WORD PTR es:8[bx]
	call	FAR PTR __aFulrem
	add	ax,OFFSET 1
	adc	dx,OFFSET 0
	and	ax,OFFSET 255
	and	dx,OFFSET 0
	mov	BYTE PTR -4[bp],al
; Line 1047
	push	WORD PTR 20[bp]
	push	WORD PTR 18[bp]
	push	WORD PTR 24[bp]
	push	WORD PTR 22[bp]
	les	bx,WORD PTR 6[bp]
	push	WORD PTR es:10[bx]
	push	WORD PTR es:8[bx]
	call	FAR PTR __aFuldiv
	push	dx
	push	ax
	call	FAR PTR __aFuldiv
	mov	WORD PTR -8[bp],ax
	mov	WORD PTR -6[bp],dx
; Line 1048
	cmp	WORD PTR -6[bp],OFFSET 0
	jb	L02096
	ja	L02379
	cmp	WORD PTR -8[bp],OFFSET 1024
	jbe	L02096
L02379:
; Line 1050
	mov	ax,OFFSET 1024
	mov	dx,OFFSET 0
	push	dx
	push	ax
	push	WORD PTR -6[bp]
	push	WORD PTR -8[bp]
	call	FAR PTR __aFulrem
	mov	WORD PTR -8[bp],ax
	mov	WORD PTR -6[bp],dx
; Line 1051
L02096:
	les	bx,WORD PTR 6[bp]
	mov	al,BYTE PTR -10[bp]
	mov	BYTE PTR es:1[bx],al
; Line 1052
	mov	al,BYTE PTR -4[bp]
	and	ax,OFFSET 255
	cwd
	and	ax,WORD PTR 22[bp]
	and	dx,WORD PTR 24[bp]
	mov	cx,WORD PTR -8[bp]
	mov	bx,WORD PTR -6[bp]
	and	cx,OFFSET 768
	and	bx,OFFSET 0
	mov	WORD PTR -18[bp],ax
	mov	WORD PTR -16[bp],dx
	mov	ax,cx
	mov	dx,bx
	mov	cx,OFFSET 2
	call	FAR PTR __aFulshr
	mov	dx,OFFSET 0
	mov	cx,WORD PTR -18[bp]
	mov	bx,WORD PTR -16[bp]
	or	ax,cx
	or	dx,bx
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:2[bx],al
; Line 1053
	mov	ax,WORD PTR -8[bp]
	mov	dx,WORD PTR -6[bp]
	and	ax,OFFSET -1
	and	dx,OFFSET 0
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:3[bx],al
; Line 1055
	push	WORD PTR 20[bp]
	push	WORD PTR 18[bp]
	push	WORD PTR 24[bp]
	push	WORD PTR 22[bp]
	push	WORD PTR -12[bp]
	push	WORD PTR -14[bp]
	call	FAR PTR __aFuldiv
	push	dx
	push	ax
	call	FAR PTR __aFulrem
	and	ax,OFFSET 255
	and	dx,OFFSET 0
	mov	BYTE PTR -10[bp],al
; Line 1056
	push	WORD PTR 24[bp]
	push	WORD PTR 22[bp]
	push	WORD PTR -12[bp]
	push	WORD PTR -14[bp]
	call	FAR PTR __aFulrem
	add	ax,OFFSET 1
	adc	dx,OFFSET 0
	and	ax,OFFSET 255
	and	dx,OFFSET 0
	mov	BYTE PTR -4[bp],al
; Line 1057
	push	WORD PTR 20[bp]
	push	WORD PTR 18[bp]
	push	WORD PTR 24[bp]
	push	WORD PTR 22[bp]
	push	WORD PTR -12[bp]
	push	WORD PTR -14[bp]
	call	FAR PTR __aFuldiv
	push	dx
	push	ax
	call	FAR PTR __aFuldiv
	mov	WORD PTR -8[bp],ax
	mov	WORD PTR -6[bp],dx
; Line 1058
	mov	ax,WORD PTR 38[bp]
	mov	dx,WORD PTR 40[bp]
	jmp	L02103
; Line 1059
; Line 1060
L02107:
; Line 1061
	les	bx,WORD PTR 6[bp]
	mov	al,BYTE PTR es:4[bx]
	and	ax,OFFSET 255
	jmp	L02108
; Line 1062
; Line 1063
L02112:
; Line 1064
; Line 1065
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 6
; Line 1066
	jmp	L02109
; Line 1067
L02113:
; Line 1068
; Line 1069
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 22
; Line 1070
	jmp	L02109
; Line 1071
L02114:
; Line 1072
; Line 1073
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 11
; Line 1074
	jmp	L02109
; Line 1075
L02115:
; Line 1076
; Line 1077
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 27
; Line 1078
L02116:
; Line 1079
	jmp	L02109
; Line 1080
; Line 1080
	jmp	L02109
L02108:
	cmp	ax,OFFSET 6
	je	L02112
	cmp	ax,OFFSET 11
	je	L02114
	cmp	ax,OFFSET 12
	je	L02114
	cmp	ax,OFFSET 14
	je	L02112
	cmp	ax,OFFSET 22
	je	L02113
	cmp	ax,OFFSET 27
	je	L02115
	cmp	ax,OFFSET 28
	je	L02115
	cmp	ax,OFFSET 30
	je	L02113
	jmp	L02116
L02109:
; Line 1081
	jmp	L02104
; Line 1082
L02117:
; Line 1083
	les	bx,WORD PTR 6[bp]
	mov	al,BYTE PTR es:4[bx]
	and	ax,OFFSET 255
	jmp	L02118
; Line 1084
; Line 1085
L02122:
; Line 1086
; Line 1087
	cmp	WORD PTR -6[bp],OFFSET 0
	jb	L02123
	ja	L02380
	cmp	WORD PTR -8[bp],OFFSET 1024
	jb	L02123
L02380:
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 14
; Line 1088
	jmp	L02124
L02123:
; Line 1088
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 6
L02124:
; Line 1089
	jmp	L02119
; Line 1090
L02125:
; Line 1091
; Line 1092
	cmp	WORD PTR -6[bp],OFFSET 0
	jb	L02126
	ja	L02381
	cmp	WORD PTR -8[bp],OFFSET 1024
	jb	L02126
L02381:
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 30
; Line 1093
	jmp	L02127
L02126:
; Line 1093
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 22
L02127:
; Line 1094
	jmp	L02119
; Line 1095
L02128:
; Line 1096
; Line 1097
	cmp	WORD PTR -6[bp],OFFSET 0
	jb	L02129
	ja	L02382
	cmp	WORD PTR -8[bp],OFFSET 1024
	jb	L02129
L02382:
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 12
; Line 1098
	jmp	L02130
L02129:
; Line 1098
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 11
L02130:
; Line 1099
	jmp	L02119
; Line 1100
L02131:
; Line 1101
; Line 1102
	cmp	WORD PTR -6[bp],OFFSET 0
	jb	L02132
	ja	L02383
	cmp	WORD PTR -8[bp],OFFSET 1024
	jb	L02132
L02383:
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 28
; Line 1103
	jmp	L02133
L02132:
; Line 1103
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 27
L02133:
; Line 1104
L02134:
; Line 1105
	jmp	L02119
; Line 1106
; Line 1106
	jmp	L02119
L02118:
	cmp	ax,OFFSET 6
	je	L02122
	cmp	ax,OFFSET 11
	je	L02128
	cmp	ax,OFFSET 12
	je	L02128
	cmp	ax,OFFSET 14
	je	L02122
	cmp	ax,OFFSET 22
	je	L02125
	cmp	ax,OFFSET 27
	je	L02131
	cmp	ax,OFFSET 28
	je	L02131
	cmp	ax,OFFSET 30
	je	L02125
	jmp	L02134
L02119:
; Line 1107
	jmp	L02104
; Line 1108
L02135:
; Line 1109
	cmp	WORD PTR -6[bp],OFFSET 0
	jb	L02136
	ja	L02384
	cmp	WORD PTR -8[bp],OFFSET 1024
	jb	L02136
L02384:
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 15
; Line 1110
	jmp	L02137
L02136:
; Line 1110
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:4[bx],OFFSET 5
L02137:
; Line 1111
L02138:
; Line 1112
	jmp	L02104
; Line 1113
; Line 1113
	jmp	L02104
L02103:
	cmp	ax,OFFSET 1
	jne	L02385
	cmp	dx,OFFSET 0
	je	L02117
L02385:
	cmp	ax,OFFSET 2
	jne	L02386
	cmp	dx,OFFSET 0
	je	L02135
L02386:
	cmp	ax,OFFSET 3
	jne	L02387
	cmp	dx,OFFSET 0
	je	L02107
L02387:
	jmp	L02138
L02104:
; Line 1115
	cmp	WORD PTR -6[bp],OFFSET 0
	jb	L02139
	ja	L02388
	cmp	WORD PTR -8[bp],OFFSET 1024
	jbe	L02139
L02388:
; Line 1116
; Line 1118
	mov	ax,OFFSET 1024
	mov	dx,OFFSET 0
	push	dx
	push	ax
	push	WORD PTR -6[bp]
	push	WORD PTR -8[bp]
	call	FAR PTR __aFulrem
	mov	WORD PTR -8[bp],ax
	mov	WORD PTR -6[bp],dx
; Line 1119
; Line 1120
L02139:
	les	bx,WORD PTR 6[bp]
	mov	al,BYTE PTR -10[bp]
	mov	BYTE PTR es:5[bx],al
; Line 1121
	mov	al,BYTE PTR -4[bp]
	and	ax,OFFSET 255
	cwd
	and	ax,WORD PTR 22[bp]
	and	dx,WORD PTR 24[bp]
	mov	cx,WORD PTR -8[bp]
	mov	bx,WORD PTR -6[bp]
	and	cx,OFFSET 768
	and	bx,OFFSET 0
	mov	WORD PTR -18[bp],ax
	mov	WORD PTR -16[bp],dx
	mov	ax,cx
	mov	dx,bx
	mov	cx,OFFSET 2
	call	FAR PTR __aFulshr
	mov	dx,OFFSET 0
	mov	cx,WORD PTR -18[bp]
	mov	bx,WORD PTR -16[bp]
	or	ax,cx
	or	dx,bx
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:6[bx],al
; Line 1122
	mov	ax,WORD PTR -8[bp]
	mov	dx,WORD PTR -6[bp]
	and	ax,OFFSET -1
	and	dx,OFFSET 0
	les	bx,WORD PTR 6[bp]
	mov	BYTE PTR es:7[bx],al
; Line 1123
	mov	ax,OFFSET 1
	jmp	L02086
; Line 1124
; Line 1124
L02086:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 1126
; btHardDisk = 0006
; peParEntry = 0008
; pnErr = 000c
_DoResizeExtendPartition:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02389
	call	FAR PTR __aFchkstk
	push	si
	push	di
; PartitionLogicalMBR = fdfe
; i = fdfc
; PartitionMBR = fbfc
; bResult = fbfa
; DriveParam = fbe0
; nExtend = fbde
; dwTemp = fbda
; ParHard = f8be
; Line 1127
; Line 1128
	mov	WORD PTR -1030[bp],OFFSET 0
; Line 1129
; Line 1130
; Line 1131
; Line 1132
	mov	WORD PTR -1062[bp],OFFSET 0
	mov	WORD PTR -1060[bp],OFFSET 0
; Line 1133
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],OFFSET 0
; Line 1135
	lea	ax,WORD PTR -1056[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetDriveParam
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L02156
; Line 1136
; Line 1137
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],OFFSET 102
; Line 1138
	mov	ax,OFFSET 0
	jmp	L02147
; Line 1139
; Line 1140
L02156:
	push	WORD PTR -1042[bp]
	push	WORD PTR -1044[bp]
	push	WORD PTR -1046[bp]
	push	WORD PTR -1048[bp]
	call	FAR PTR __aFulmul
	les	bx,WORD PTR 8[bp]
	cmp	WORD PTR es:14[bx],dx
	ja	L02157
	jb	L02390
	cmp	WORD PTR es:12[bx],ax
	jae	L02157
L02390:
; Line 1141
; Line 1142
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],OFFSET 105
; Line 1143
	mov	ax,OFFSET 0
	jmp	L02147
; Line 1144
; Line 1145
L02157:
	push	WORD PTR -1042[bp]
	push	WORD PTR -1044[bp]
	push	WORD PTR -1046[bp]
	push	WORD PTR -1048[bp]
	call	FAR PTR __aFulmul
	les	bx,WORD PTR 8[bp]
	cmp	WORD PTR es:10[bx],dx
	ja	L02158
	jb	L02391
	cmp	WORD PTR es:8[bx],ax
	jae	L02158
L02391:
; Line 1146
; Line 1147
	push	WORD PTR -1042[bp]
	push	WORD PTR -1044[bp]
	push	WORD PTR -1046[bp]
	push	WORD PTR -1048[bp]
	call	FAR PTR __aFulmul
	les	bx,WORD PTR 8[bp]
	add	WORD PTR es:8[bx],ax
	adc	WORD PTR es:10[bx],dx
; Line 1148
	push	WORD PTR -1042[bp]
	push	WORD PTR -1044[bp]
	push	WORD PTR -1046[bp]
	push	WORD PTR -1048[bp]
	call	FAR PTR __aFulmul
	les	bx,WORD PTR 8[bp]
	mov	cx,WORD PTR es:12[bx]
	mov	bx,WORD PTR es:14[bx]
	sub	cx,ax
	sbb	bx,dx
	mov	WORD PTR -1862[bp],cx
	mov	WORD PTR -1860[bp],bx
	les	bx,WORD PTR 8[bp]
	mov	ax,WORD PTR -1862[bp]
	mov	dx,WORD PTR -1860[bp]
	mov	WORD PTR es:12[bx],ax
	mov	WORD PTR es:14[bx],dx
; Line 1149
; Line 1151
L02158:
	push	OFFSET 0
	push	OFFSET 2
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	sub	sp,OFFSET 26
	lea	si,WORD PTR -1056[bp]
	mov	di,sp
	push	ss
	pop	es
	mov	cx,OFFSET 13
	rep
	movs
	push	WORD PTR 10[bp]
	push	WORD PTR 8[bp]
	call	FAR PTR _CaculateStartAndEnd
	add	sp,OFFSET 36
; Line 1153
	push	OFFSET 588
	push	OFFSET 0
	lea	ax,WORD PTR -1858[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 1154
	lea	ax,WORD PTR -1858[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetPartitionInfoEx
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L02160
; Line 1155
; Line 1156
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],OFFSET 106
; Line 1157
	mov	ax,OFFSET 0
	jmp	L02147
; Line 1158
; Line 1159
L02160:
	lea	ax,WORD PTR -1056[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -1028[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L02162
; Line 1160
; Line 1161
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],OFFSET 103
; Line 1162
	mov	ax,OFFSET 0
	jmp	L02147
; Line 1163
; Line 1164
L02162:
	mov	WORD PTR -516[bp],OFFSET 0
	jmp	L02166
L02163:
; Line 1165
; Line 1166
; Line 1167
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 5
	je	L02168
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 15
	jne	L02167
L02168:
; Line 1168
; Line 1169
	mov	ax,WORD PTR -516[bp]
	mov	WORD PTR -1058[bp],ax
; Line 1170
	cmp	WORD PTR -1856[bp],OFFSET 0
	je	L02169
; Line 1171
; Line 1172
	les	bx,WORD PTR 8[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 8[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	cx,WORD PTR -1856[bp]
	add	cx,OFFSET -1
	mov	WORD PTR -1862[bp],ax
	mov	WORD PTR -1860[bp],dx
	mov	ax,cx
	mov	cx,OFFSET 28
	imul	cx
	lea	bx,WORD PTR -1790[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:20[bx]
	mov	dx,WORD PTR ss:22[bx]
	mov	cx,WORD PTR -1856[bp]
	add	cx,OFFSET -1
	mov	WORD PTR -1866[bp],ax
	mov	WORD PTR -1864[bp],dx
	mov	ax,cx
	mov	cx,OFFSET 28
	imul	cx
	lea	bx,WORD PTR -1790[bp]
	add	bx,ax
	mov	ax,WORD PTR -1866[bp]
	mov	dx,WORD PTR -1864[bp]
	add	ax,WORD PTR ss:24[bx]
	adc	dx,WORD PTR ss:26[bx]
	mov	cx,WORD PTR -1862[bp]
	mov	bx,WORD PTR -1860[bp]
	cmp	bx,dx
	ja	L02170
	jb	L02392
	cmp	cx,ax
	jae	L02170
L02392:
; Line 1173
; Line 1174
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],OFFSET 111
; Line 1175
	mov	ax,OFFSET 0
	jmp	L02147
; Line 1176
; Line 1177
L02170:
; Line 1178
L02169:
; Line 1179
	jmp	L02171
L02167:
; Line 1179
; Line 1180
; Line 1181
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L02172
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	cmp	WORD PTR ss:12[bx],OFFSET 0
	jne	L02393
	cmp	WORD PTR ss:14[bx],OFFSET 0
	je	L02172
L02393:
; Line 1182
; Line 1183
; Line 1184
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	les	bx,WORD PTR 8[bp]
	cmp	WORD PTR es:10[bx],dx
	jb	L02173
	ja	L02394
	cmp	WORD PTR es:8[bx],ax
	jb	L02173
L02394:
	mov	ax,WORD PTR -516[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	bx,WORD PTR -516[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -582[bp]
	add	si,bx
	add	ax,WORD PTR ss:12[si]
	adc	dx,WORD PTR ss:14[si]
	les	bx,WORD PTR 8[bp]
	cmp	WORD PTR es:10[bx],dx
	ja	L02173
	jb	L02395
	cmp	WORD PTR es:8[bx],ax
	jae	L02173
L02395:
; Line 1185
; Line 1186
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],OFFSET 109
; Line 1187
	mov	ax,OFFSET 0
	jmp	L02147
; Line 1188
; Line 1189
L02173:
; Line 1190
	les	bx,WORD PTR 8[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 8[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	bx,WORD PTR -516[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -582[bp]
	add	si,bx
	cmp	dx,WORD PTR ss:10[si]
	jb	L02174
	ja	L02396
	cmp	ax,WORD PTR ss:8[si]
	jbe	L02174
L02396:
	les	bx,WORD PTR 8[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 8[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	bx,WORD PTR -516[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -582[bp]
	add	si,bx
	mov	cx,WORD PTR ss:8[si]
	mov	bx,WORD PTR ss:10[si]
	mov	si,WORD PTR -516[bp]
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	shl	si,OFFSET 1
	lea	di,WORD PTR -582[bp]
	add	di,si
	add	cx,WORD PTR ss:12[di]
	adc	bx,WORD PTR ss:14[di]
	cmp	dx,bx
	ja	L02174
	jb	L02397
	cmp	ax,cx
	ja	L02174
L02397:
; Line 1191
; Line 1192
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],OFFSET 109
; Line 1193
	mov	ax,OFFSET 0
	jmp	L02147
; Line 1194
; Line 1195
L02174:
; Line 1196
L02172:
L02171:
; Line 1197
L02164:
; Line 1197
	add	WORD PTR -516[bp],OFFSET 1
L02166:
	cmp	WORD PTR -516[bp],OFFSET 4
	jl	L02163
L02165:
; Line 1199
	push	OFFSET 16
	push	WORD PTR 10[bp]
	push	WORD PTR 8[bp]
	mov	ax,WORD PTR -1058[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	cx,WORD PTR -582[bp]
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 1200
	lea	ax,WORD PTR -1056[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -1028[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	je	L02177
; Line 1201
; Line 1202
	mov	ax,WORD PTR -1058[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	WORD PTR -1062[bp],ax
	mov	WORD PTR -1060[bp],dx
; Line 1203
	push	OFFSET 512
	push	OFFSET 0
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 1204
	mov	WORD PTR -4[bp],OFFSET -21931
; Line 1205
	lea	ax,WORD PTR -1056[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR -1060[bp]
	push	WORD PTR -1062[bp]
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
; Line 1206
	mov	WORD PTR -516[bp],OFFSET 0
	jmp	L02180
L02181:
	add	WORD PTR -516[bp],OFFSET 1
L02180:
	mov	ax,WORD PTR -516[bp]
	cmp	ax,WORD PTR -1856[bp]
	jae	L02182
; Line 1207
; Line 1208
	cmp	WORD PTR -516[bp],OFFSET 0
	je	L02184
; Line 1209
; Line 1210
	push	OFFSET 16
	mov	ax,WORD PTR -516[bp]
	add	ax,OFFSET -1
	_imul3 	ax,ax,OFFSET 28
	lea	cx,WORD PTR -1790[bp]
	add	cx,ax
	mov	ax,OFFSET 12
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	lea	ax,WORD PTR -68[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 1211
	push	OFFSET 16
	_imul3 	ax,WORD PTR -516[bp],OFFSET 28
	lea	cx,WORD PTR -1790[bp]
	add	cx,ax
	mov	ax,OFFSET 12
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	lea	ax,WORD PTR -52[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 1212
	mov	ax,WORD PTR -516[bp]
	add	ax,OFFSET -1
	_imul3 	ax,ax,OFFSET 28
	lea	bx,WORD PTR -1790[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	WORD PTR -60[bp],ax
	mov	WORD PTR -58[bp],dx
; Line 1213
	_imul3 	ax,WORD PTR -516[bp],OFFSET 28
	lea	bx,WORD PTR -1790[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	add	WORD PTR -40[bp],ax
	adc	WORD PTR -38[bp],dx
; Line 1214
; Line 1215
	jmp	L02187
L02184:
; Line 1215
; Line 1216
; Line 1217
	_imul3 	ax,WORD PTR -516[bp],OFFSET 28
	lea	bx,WORD PTR -1790[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:20[bx]
	mov	dx,WORD PTR ss:22[bx]
	mov	bx,WORD PTR -1058[bp]
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	shl	bx,OFFSET 1
	lea	si,WORD PTR -582[bp]
	add	si,bx
	cmp	WORD PTR ss:8[si],ax
	jne	L02188
	cmp	WORD PTR ss:10[si],dx
	jne	L02188
; Line 1218
; Line 1219
	push	OFFSET 16
	_imul3 	ax,WORD PTR -516[bp],OFFSET 28
	lea	cx,WORD PTR -1790[bp]
	add	cx,ax
	mov	ax,OFFSET 12
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	lea	ax,WORD PTR -68[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 1220
	push	OFFSET 16
	mov	ax,WORD PTR -516[bp]
	add	ax,OFFSET 1
	_imul3 	ax,ax,OFFSET 28
	lea	cx,WORD PTR -1790[bp]
	add	cx,ax
	mov	ax,OFFSET 12
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	lea	ax,WORD PTR -52[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 1221
	_imul3 	ax,WORD PTR -516[bp],OFFSET 28
	lea	bx,WORD PTR -1790[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	WORD PTR -60[bp],ax
	mov	WORD PTR -58[bp],dx
; Line 1222
	mov	ax,WORD PTR -516[bp]
	add	ax,OFFSET 1
	_imul3 	ax,ax,OFFSET 28
	lea	bx,WORD PTR -1790[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	add	WORD PTR -40[bp],ax
	adc	WORD PTR -38[bp],dx
; Line 1223
; Line 1224
	jmp	L02191
L02188:
; Line 1224
; Line 1225
; Line 1226
	push	OFFSET 16
	_imul3 	ax,WORD PTR -516[bp],OFFSET 28
	lea	cx,WORD PTR -1790[bp]
	add	cx,ax
	mov	ax,OFFSET 12
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	lea	ax,WORD PTR -52[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 1227
	_imul3 	ax,WORD PTR -516[bp],OFFSET 28
	lea	bx,WORD PTR -1790[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	add	WORD PTR -40[bp],ax
	adc	WORD PTR -38[bp],dx
; Line 1228
L02191:
; Line 1229
L02187:
; Line 1230
	mov	ax,WORD PTR -1058[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -582[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	sub	WORD PTR -44[bp],ax
	sbb	WORD PTR -42[bp],dx
; Line 1231
	mov	BYTE PTR -51[bp],OFFSET 0
; Line 1232
	mov	al,BYTE PTR -48[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	je	L02193
; Line 1233
	mov	BYTE PTR -48[bp],OFFSET 5
; Line 1234
L02193:
	lea	ax,WORD PTR -1056[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	al,BYTE PTR 6[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR -1060[bp]
	push	WORD PTR -1062[bp]
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L02195
; Line 1235
; Line 1236
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],OFFSET 104
; Line 1237
	mov	ax,OFFSET 0
	jmp	L02147
; Line 1238
; Line 1239
L02195:
	_imul3 	ax,WORD PTR -516[bp],OFFSET 28
	lea	bx,WORD PTR -1790[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:20[bx]
	mov	dx,WORD PTR ss:22[bx]
	mov	WORD PTR -1062[bp],ax
	mov	WORD PTR -1060[bp],dx
; Line 1240
; Line 1240
	jmp	L02181
L02182:
; Line 1241
	mov	WORD PTR -1030[bp],OFFSET 1
; Line 1242
; Line 1243
	jmp	L02196
L02177:
; Line 1243
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],OFFSET 104
L02196:
; Line 1244
	mov	ax,WORD PTR -1030[bp]
	jmp	L02147
; Line 1245
; Line 1245
L02147:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 1407
; peParEntry = 0006
; btHardDisk = 000a
; DriveParam = 000c
_DoCreatePriPar:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02398
	call	FAR PTR __aFchkstk
	push	si
	push	di
; nPartition = fffc
; mbr = fdfc
; dw = fdf8
; Line 1408
; Line 1409
; Line 1410
; Line 1412
	push	WORD PTR 14[bp]
	push	WORD PTR 12[bp]
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -516[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	je	L02205
; Line 1413
; Line 1414
	mov	WORD PTR -4[bp],OFFSET 0
	jmp	L02209
L02206:
; Line 1415
; Line 1416
	mov	ax,WORD PTR -4[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -70[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	jne	L02210
; Line 1417
; Line 1418
	push	OFFSET 16
	push	WORD PTR 8[bp]
	push	WORD PTR 6[bp]
	mov	ax,WORD PTR -4[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	cx,WORD PTR -70[bp]
	add	cx,ax
	mov	dx,ss
	push	dx
	push	cx
	call	FAR PTR _memcpy
	add	sp,OFFSET 10
; Line 1419
	mov	ax,WORD PTR -4[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -70[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	mov	WORD PTR -520[bp],ax
	mov	WORD PTR -518[bp],dx
; Line 1420
	push	WORD PTR 14[bp]
	push	WORD PTR 12[bp]
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -516[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	OFFSET 0
	push	OFFSET 0
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	je	L02213
; Line 1421
; Line 1422
	les	bx,WORD PTR 6[bp]
	mov	al,BYTE PTR es:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 5
	je	L02215
	les	bx,WORD PTR 6[bp]
	mov	al,BYTE PTR es:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 15
	jne	L02214
L02215:
; Line 1423
; Line 1424
	push	OFFSET 512
	push	OFFSET 0
	lea	ax,WORD PTR -516[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 1425
	mov	WORD PTR -6[bp],OFFSET -21931
; Line 1426
	push	WORD PTR 14[bp]
	push	WORD PTR 12[bp]
	mov	al,BYTE PTR 10[bp]
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -516[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	push	WORD PTR -518[bp]
	push	WORD PTR -520[bp]
	call	FAR PTR _WriteSector
	add	sp,OFFSET 16
; Line 1427
; Line 1428
L02214:
	mov	ax,OFFSET 1
	jmp	L02200
; Line 1429
; Line 1430
L02213:
; Line 1431
L02210:
L02207:
; Line 1431
	add	WORD PTR -4[bp],OFFSET 1
L02209:
	cmp	WORD PTR -4[bp],OFFSET 4
	jl	L02206
L02208:
; Line 1432
; Line 1433
L02205:
	mov	ax,OFFSET 0
	jmp	L02200
; Line 1434
; Line 1434
L02200:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 1437
; ph = 0006
; pe = 000a
_VerifyPrimaryPartition:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02399
	call	FAR PTR __aFchkstk
	push	si
	push	di
; i = fffc
; Line 1438
	mov	WORD PTR -4[bp],OFFSET 0
	jmp	L02222
L02223:
	add	WORD PTR -4[bp],OFFSET 1
L02222:
	mov	ax,WORD PTR -4[bp]
	les	bx,WORD PTR 6[bp]
	cmp	ax,WORD PTR es:2[bx]
	jae	L02224
; Line 1439
; Line 1440
; Line 1441
	mov	ax,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	cx,WORD PTR 8[bp]
	add	bx,ax
	_imul3 	ax,WORD PTR -4[bp],OFFSET 28
	add	bx,ax
	mov	es,cx
	mov	ax,WORD PTR es:20[bx]
	mov	dx,WORD PTR es:22[bx]
	les	bx,WORD PTR 10[bp]
	cmp	WORD PTR es:10[bx],dx
	jb	L02226
	ja	L02400
	cmp	WORD PTR es:8[bx],ax
	jb	L02226
L02400:
	mov	ax,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	cx,WORD PTR 8[bp]
	add	bx,ax
	_imul3 	ax,WORD PTR -4[bp],OFFSET 28
	add	bx,ax
	mov	es,cx
	mov	ax,WORD PTR es:20[bx]
	mov	dx,WORD PTR es:22[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	WORD PTR -8[bp],ax
	mov	WORD PTR -6[bp],dx
	_imul3 	ax,WORD PTR -4[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -8[bp]
	mov	dx,WORD PTR -6[bp]
	mov	es,si
	add	ax,WORD PTR es:24[bx]
	adc	dx,WORD PTR es:26[bx]
	les	bx,WORD PTR 10[bp]
	cmp	WORD PTR es:10[bx],dx
	ja	L02226
	jb	L02401
	cmp	WORD PTR es:8[bx],ax
	jae	L02226
L02401:
; Line 1442
	mov	ax,OFFSET 109
	jmp	L02220
; Line 1443
L02226:
; Line 1444
	les	bx,WORD PTR 10[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 10[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	WORD PTR -8[bp],ax
	mov	WORD PTR -6[bp],dx
	_imul3 	ax,WORD PTR -4[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -8[bp]
	mov	dx,WORD PTR -6[bp]
	mov	es,si
	cmp	dx,WORD PTR es:22[bx]
	jb	L02227
	ja	L02402
	cmp	ax,WORD PTR es:20[bx]
	jbe	L02227
L02402:
	les	bx,WORD PTR 10[bp]
	mov	ax,WORD PTR es:8[bx]
	mov	dx,WORD PTR es:10[bx]
	les	bx,WORD PTR 10[bp]
	add	ax,WORD PTR es:12[bx]
	adc	dx,WORD PTR es:14[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	WORD PTR -8[bp],ax
	mov	WORD PTR -6[bp],dx
	_imul3 	ax,WORD PTR -4[bp],OFFSET 28
	add	bx,ax
	mov	es,si
	mov	ax,WORD PTR es:20[bx]
	mov	dx,WORD PTR es:22[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	WORD PTR -12[bp],ax
	mov	WORD PTR -10[bp],dx
	_imul3 	ax,WORD PTR -4[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -12[bp]
	mov	dx,WORD PTR -10[bp]
	mov	es,si
	add	ax,WORD PTR es:24[bx]
	adc	dx,WORD PTR es:26[bx]
	mov	cx,WORD PTR -8[bp]
	mov	bx,WORD PTR -6[bp]
	cmp	bx,dx
	ja	L02227
	jb	L02403
	cmp	cx,ax
	jae	L02227
L02403:
; Line 1445
	mov	ax,OFFSET 109
	jmp	L02220
; Line 1446
L02227:
; Line 1447
	mov	ax,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	cx,WORD PTR 8[bp]
	add	bx,ax
	_imul3 	ax,WORD PTR -4[bp],OFFSET 28
	add	bx,ax
	mov	es,cx
	mov	ax,WORD PTR es:20[bx]
	mov	dx,WORD PTR es:22[bx]
	mov	cx,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	si,WORD PTR 8[bp]
	add	bx,cx
	mov	WORD PTR -8[bp],ax
	mov	WORD PTR -6[bp],dx
	_imul3 	ax,WORD PTR -4[bp],OFFSET 28
	add	bx,ax
	mov	ax,WORD PTR -8[bp]
	mov	dx,WORD PTR -6[bp]
	mov	es,si
	add	ax,WORD PTR es:24[bx]
	adc	dx,WORD PTR es:26[bx]
	les	bx,WORD PTR 10[bp]
	cmp	WORD PTR es:10[bx],dx
	jb	L02228
	ja	L02404
	cmp	WORD PTR es:8[bx],ax
	jb	L02228
L02404:
	mov	ax,OFFSET 68
	mov	bx,WORD PTR 6[bp]
	mov	cx,WORD PTR 8[bp]
	add	bx,ax
	_imul3 	ax,WORD PTR -4[bp],OFFSET 28
	add	bx,ax
	mov	es,cx
	mov	ax,WORD PTR es:4[bx]
	mov	dx,WORD PTR es:6[bx]
	les	bx,WORD PTR 10[bp]
	cmp	WORD PTR es:10[bx],dx
	ja	L02228
	jb	L02405
	cmp	WORD PTR es:8[bx],ax
	jae	L02228
L02405:
; Line 1448
; Line 1449
	mov	ax,OFFSET 109
	jmp	L02220
; Line 1450
; Line 1451
L02228:
; Line 1451
	jmp	L02223
L02224:
; Line 1452
	mov	ax,OFFSET 0
	jmp	L02220
; Line 1453
; Line 1453
L02220:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 1456
; nDrive = 0006
; p = 0008
_GetDriveMapInfo:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02406
	call	FAR PTR __aFchkstk
	push	si
	push	di
; dwStartSec = fffa
; HardDrive = fff8
; Line 1457
	mov	WORD PTR -8[bp],OFFSET 0
; Line 1458
	mov	WORD PTR -6[bp],OFFSET 0
	mov	WORD PTR -4[bp],OFFSET 0
; Line 1459
	lea	ax,WORD PTR -8[bp]
	mov	dx,ss
	push	dx
	push	ax
	lea	ax,WORD PTR -6[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	WORD PTR 6[bp]
	call	FAR PTR _GetStartSector
	add	sp,OFFSET 10
	cmp	ax,OFFSET 0
	jne	L02234
; Line 1460
; Line 1461
	les	bx,WORD PTR 8[bp]
	mov	BYTE PTR es:3[bx],OFFSET 0
; Line 1462
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:8[bx],OFFSET 0
	mov	WORD PTR es:10[bx],OFFSET 0
; Line 1463
	mov	ax,OFFSET 0
	jmp	L02231
; Line 1464
; Line 1465
L02234:
	mov	ax,WORD PTR -8[bp]
	les	bx,WORD PTR 8[bp]
	mov	BYTE PTR es:3[bx],al
; Line 1466
	les	bx,WORD PTR 8[bp]
	mov	ax,WORD PTR -6[bp]
	mov	dx,WORD PTR -4[bp]
	mov	WORD PTR es:8[bx],ax
	mov	WORD PTR es:10[bx],dx
; Line 1467
	mov	ax,OFFSET 1
	jmp	L02231
; Line 1468
; Line 1468
L02231:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 1471
; nDrive = 0006
; dwStartSector = 0008
; Drive = 000c
_GetStartSector:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02407
	call	FAR PTR __aFchkstk
	push	si
	push	di
; pBuf1 = fdfe
; nHardDisk = fdfc
; j = fdfa
; btResult = fdf8
; i = fdf6
; pBuf = fbf6
; pDiskIo = fbf2
; DriveParam = fbd8
; DiskIo = fbce
; ParHard = f8b2
; wOff = f8b0
; Line 1472
; Line 1473
; Line 1474
	mov	BYTE PTR -520[bp],OFFSET 1
; Line 1475
; Line 1476
; Line 1477
; Line 1478
; Line 1480
	push	OFFSET 512
	push	OFFSET 0
	lea	ax,WORD PTR -1034[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 1481
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],OFFSET 0
; Line 1482
	push	OFFSET 10
	push	OFFSET 0
	lea	ax,WORD PTR -1074[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 1484
	lea	ax,WORD PTR -1074[bp]
	mov	dx,ss
	mov	WORD PTR -1038[bp],ax
	mov	WORD PTR -1036[bp],dx
; Line 1485
	lea	bx,WORD PTR -1038[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR -1872[bp],ax
; Line 1486
	mov	WORD PTR -1074[bp],OFFSET 0
	mov	WORD PTR -1072[bp],OFFSET 0
; Line 1487
	mov	WORD PTR -1070[bp],OFFSET 1
; Line 1488
	lea	ax,WORD PTR -1034[bp]
	mov	dx,ss
	mov	WORD PTR -1068[bp],ax
	mov	WORD PTR -1066[bp],dx
; Line 1489
; Line 1490
; Line 1491
	mov	ax,OFFSET 29445
; Line 1492
	mov	bx,WORD PTR -1872[bp]
; Line 1493
	mov	cx,OFFSET -1
; Line 1494
	mov	dx,WORD PTR 6[bp]
; Line 1495
	xor	si,si
; Line 1496
	int	OFFSET 33
; Line 1497
	mov	BYTE PTR -520[bp],ah
; Line 1498
; Line 1500
	call	FAR PTR _GetHardDiskNum
	mov	WORD PTR -516[bp],ax
; Line 1501
	mov	WORD PTR -518[bp],OFFSET 0
	jmp	L02256
L02253:
; Line 1502
; Line 1503
	push	OFFSET 26
	push	OFFSET 0
	lea	ax,WORD PTR -1064[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 1504
	push	OFFSET 796
	push	OFFSET 0
	lea	ax,WORD PTR -1870[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 1505
	lea	ax,WORD PTR -1064[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	ax,WORD PTR -518[bp]
	add	ax,OFFSET 128
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetDriveParam
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L02259
	jmp	L02255
; Line 1506
L02259:
	lea	ax,WORD PTR -1870[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	ax,WORD PTR -518[bp]
	add	ax,OFFSET 128
	and	ax,OFFSET 255
	push	ax
	call	FAR PTR _GetPartitionInfoEx
	add	sp,OFFSET 6
	cmp	ax,OFFSET 0
	jne	L02260
	jmp	L02255
; Line 1507
L02260:
	mov	WORD PTR -522[bp],OFFSET 0
	jmp	L02264
L02261:
; Line 1508
; Line 1509
	mov	ax,WORD PTR -522[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1866[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 5
	je	L02265
	mov	ax,WORD PTR -522[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1866[bp]
	add	bx,ax
	mov	al,BYTE PTR ss:4[bx]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 15
	je	L02265
; Line 1510
; Line 1511
	lea	ax,WORD PTR -1064[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	ax,WORD PTR -518[bp]
	add	ax,OFFSET 128
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	mov	ax,WORD PTR -522[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1866[bp]
	add	bx,ax
	push	WORD PTR ss:10[bx]
	push	WORD PTR ss:8[bx]
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L02266
; Line 1512
	mov	ax,OFFSET 0
	jmp	L02238
; Line 1513
L02266:
	push	OFFSET 512
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	lea	ax,WORD PTR -1034[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcmp
	add	sp,OFFSET 10
	cmp	ax,OFFSET 0
	jne	L02267
; Line 1514
; Line 1515
	mov	ax,WORD PTR -518[bp]
	add	ax,OFFSET 128
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],ax
; Line 1516
	mov	ax,WORD PTR -522[bp]
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	shl	ax,OFFSET 1
	lea	bx,WORD PTR -1866[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:8[bx]
	mov	dx,WORD PTR ss:10[bx]
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:[bx],ax
	mov	WORD PTR es:2[bx],dx
; Line 1517
	mov	ax,OFFSET 1
	jmp	L02238
; Line 1518
; Line 1519
L02267:
; Line 1520
L02265:
L02262:
; Line 1520
	add	WORD PTR -522[bp],OFFSET 1
L02264:
	cmp	WORD PTR -522[bp],OFFSET 4
	jl	L02261
L02263:
; Line 1521
	mov	WORD PTR -522[bp],OFFSET 0
	jmp	L02268
L02269:
	add	WORD PTR -522[bp],OFFSET 1
L02268:
	mov	ax,WORD PTR -522[bp]
	cmp	ax,WORD PTR -1868[bp]
	jae	L02270
; Line 1522
; Line 1523
	lea	ax,WORD PTR -1064[bp]
	mov	dx,ss
	push	dx
	push	ax
	mov	ax,WORD PTR -518[bp]
	add	ax,OFFSET 128
	and	ax,OFFSET 255
	push	ax
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	push	OFFSET 1
	_imul3 	ax,WORD PTR -522[bp],OFFSET 28
	lea	bx,WORD PTR -1802[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:20[bx]
	mov	dx,WORD PTR ss:22[bx]
	add	ax,OFFSET 63
	adc	dx,OFFSET 0
	push	dx
	push	ax
	call	FAR PTR _ReadSector
	add	sp,OFFSET 16
	cmp	ax,OFFSET 0
	jne	L02272
; Line 1524
	mov	ax,OFFSET 0
	jmp	L02238
; Line 1525
L02272:
	push	OFFSET 512
	lea	ax,WORD PTR -514[bp]
	mov	dx,ss
	push	dx
	push	ax
	lea	ax,WORD PTR -1034[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR _memcmp
	add	sp,OFFSET 10
	cmp	ax,OFFSET 0
	jne	L02273
; Line 1526
; Line 1527
	mov	ax,WORD PTR -518[bp]
	add	ax,OFFSET 128
	les	bx,WORD PTR 12[bp]
	mov	WORD PTR es:[bx],ax
; Line 1528
	_imul3 	ax,WORD PTR -522[bp],OFFSET 28
	lea	bx,WORD PTR -1802[bp]
	add	bx,ax
	mov	ax,WORD PTR ss:20[bx]
	mov	dx,WORD PTR ss:22[bx]
	add	ax,OFFSET 63
	adc	dx,OFFSET 0
	les	bx,WORD PTR 8[bp]
	mov	WORD PTR es:[bx],ax
	mov	WORD PTR es:2[bx],dx
; Line 1529
	mov	ax,OFFSET 1
	jmp	L02238
; Line 1530
; Line 1531
L02273:
; Line 1531
	jmp	L02269
L02270:
; Line 1532
L02254:
; Line 1532
	add	WORD PTR -518[bp],OFFSET 1
L02256:
	mov	ax,WORD PTR -516[bp]
	cmp	WORD PTR -518[bp],ax
	jl	L02253
L02255:
; Line 1533
	mov	ax,OFFSET 0
	jmp	L02238
; Line 1534
; Line 1534
L02238:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 1537
; nDrive = 0006
; pMid = 0008
_GetMID:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02408
	call	FAR PTR __aFchkstk
	push	si
	push	di
; btResult = fffc
; wOff = fffa
; Line 1538
	mov	BYTE PTR -4[bp],OFFSET 1
; Line 1539
; Line 1540
	push	OFFSET 25
	push	OFFSET 0
	push	WORD PTR 10[bp]
	push	WORD PTR 8[bp]
	call	FAR PTR _memset
	add	sp,OFFSET 8
; Line 1541
	lea	bx,WORD PTR 8[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR -6[bp],ax
; Line 1542
; Line 1543
; Line 1544
	mov	ax,OFFSET 17421
; Line 1545
	mov	bx,WORD PTR 6[bp]
; Line 1546
	mov	cx,OFFSET 18534
; Line 1547
	mov	dx,WORD PTR -6[bp]
; Line 1548
	int	OFFSET 33
; Line 1549
	mov	BYTE PTR -4[bp],ah
; Line 1550
; Line 1551
	mov	al,BYTE PTR -4[bp]
	and	ax,OFFSET 255
	cmp	ax,OFFSET 0
	jne	L02410
	mov	ax,OFFSET 1
	jmp	L02409
L02410:
	mov	ax,OFFSET 0
L02409:
	jmp	L02276
; Line 1552
; Line 1552
L02276:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 1556
; nDrive = 0006
_IsCDRomDrive:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02411
	call	FAR PTR __aFchkstk
	push	si
	push	di
; bResult = fffc
; Line 1557
	sub	WORD PTR 6[bp],OFFSET 1
; Line 1558
	mov	WORD PTR -4[bp],OFFSET 0
; Line 1559
; Line 1560
	mov	ax,OFFSET 5387
; Line 1561
	xor	bx,bx
; Line 1562
	mov	cx,WORD PTR 6[bp]
; Line 1563
	int	OFFSET 47
; Line 1564
	cmp	bx,OFFSET -21075
; Line 1565
	jne	not_cd_drive
; Line 1566
	or	ax,ax
; Line 1567
	je	not_cd_drive
; Line 1568
	mov	WORD PTR -4[bp],OFFSET 1
; Line 1569
not_cd_drive:
; Line 1570
; Line 1571
	mov	ax,WORD PTR -4[bp]
	jmp	L02283
; Line 1572
; Line 1572
L02283:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 1576
; nDrive = 0006
_IsFloppyDisk:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02412
	call	FAR PTR __aFchkstk
	push	si
	push	di
; dpb = ffd8
; bResult = ffd6
; pdpb = ffd2
; wOff = ffd0
; Line 1577
	mov	WORD PTR -42[bp],OFFSET 0
; Line 1578
; Line 1579
; Line 1580
	lea	ax,WORD PTR -40[bp]
	mov	dx,ss
	mov	WORD PTR -46[bp],ax
	mov	WORD PTR -44[bp],dx
; Line 1581
	lea	bx,WORD PTR -46[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR -48[bp],ax
; Line 1582
	mov	BYTE PTR -40[bp],OFFSET 0
; Line 1583
; Line 1584
; Line 1585
	mov	ax,OFFSET 17421
; Line 1586
	mov	bx,WORD PTR 6[bp]
; Line 1587
	mov	cx,OFFSET 2144
; Line 1588
	mov	dx,WORD PTR -48[bp]
; Line 1589
	int	OFFSET 33
; Line 1590
; Line 1591
	mov	al,BYTE PTR -39[bp]
	and	ax,OFFSET 255
	jmp	L02293
; Line 1592
; Line 1593
L02297:
; Line 1594
; Line 1595
; Line 1596
	mov	WORD PTR -42[bp],OFFSET 1
	jmp	L02294
; Line 1597
L02298:
; Line 1598
	jmp	L02294
; Line 1599
; Line 1599
	jmp	L02294
L02293:
	cmp	ax,OFFSET 2
	je	L02297
	cmp	ax,OFFSET 7
	je	L02297
	cmp	ax,OFFSET 9
	je	L02297
	jmp	L02298
L02294:
; Line 1600
	mov	ax,WORD PTR -42[bp]
	jmp	L02287
; Line 1601
; Line 1601
L02287:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 1604
; Drive = 0006
_GetDriveFreeSpace:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02413
	call	FAR PTR __aFchkstk
	push	si
	push	di
; wSize = fffc
; wOffDrive = fffa
; pDiskFree = fff6
; wSeg = fff4
; DiskFree = ffc8
; wOff = ffc6
; Line 1605
; Line 1606
; Line 1608
	push	OFFSET 44
	push	OFFSET 0
	lea	ax,WORD PTR -56[bp]
	mov	dx,ss
	push	dx
	push	ax
	call	FAR PTR __fmemset
	add	sp,OFFSET 8
; Line 1609
	mov	WORD PTR -56[bp],OFFSET 44
; Line 1610
	lea	bx,WORD PTR 6[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR -6[bp],ax
; Line 1611
	lea	ax,WORD PTR -56[bp]
	mov	dx,ss
	mov	WORD PTR -10[bp],ax
	mov	WORD PTR -8[bp],dx
; Line 1612
	lea	bx,WORD PTR -10[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:[bx]
	mov	WORD PTR -58[bp],ax
; Line 1613
	lea	bx,WORD PTR -10[bp]
	mov	cx,ss
	mov	es,cx
	mov	ax,WORD PTR es:2[bx]
	mov	WORD PTR -12[bp],ax
; Line 1614
	mov	WORD PTR -4[bp],OFFSET 44
; Line 1615
; Line 1616
; Line 1617
	push	es
; Line 1618
	mov	dx,WORD PTR -12[bp]
; Line 1619
	mov	es,dx
; Line 1620
	mov	di,WORD PTR -58[bp]
; Line 1622
	mov	cx,WORD PTR -4[bp]
; Line 1623
	mov	dx,WORD PTR -6[bp]
; Line 1624
	mov	ax,OFFSET 29443
; Line 1625
	int	OFFSET 33
; Line 1626
	pop	es
; Line 1627
; Line 1628
	mov	ax,WORD PTR -36[bp]
	mov	dx,WORD PTR -34[bp]
	jmp	L02300
; Line 1629
; Line 1629
L02300:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
; Line 1633
_RestartComputer:
	push	bp
	mov	bp,sp
	mov	ax,OFFSET L02414
	call	FAR PTR __aFchkstk
	push	si
	push	di
; Line 1634
; Line 1635
; Line 1636
	mov	ax,OFFSET 64
; Line 1637
	push	ax
; Line 1638
	pop	ds
; Line 1639
	mov	WORD PTR 114,OFFSET 4660
; Line 1640
	mov	ax,OFFSET -1
; Line 1641
	push	ax
; Line 1642
	mov	ax,OFFSET 0
; Line 1643
	push	ax
; Line 1644
	ret	OFFSET 0
; Line 1645
; Line 1647
	jmp	L02314
; Line 1647
L02314:
	pop	di
	pop	si
	leave
	ret	OFFSET 0
