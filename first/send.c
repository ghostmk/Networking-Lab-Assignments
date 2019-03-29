#include<stdio.h>
#include<dos.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>
unsigned char dest_mac[6] = "\x08\x00\x27\x19\x44\x41";
unsigned char opt,packet[64],source_mac[6],num[2],in_packet[200],buffer[50],c[2],naya_buffer[20];
unsigned char type[] = "\xff\xff";
int j=0,cc,len_buf,lit=0,i,handle,fact,in_packet_len,y=0;
unsigned long long int n,k,x;
void interrupt receiver(unsigned short bp,unsigned short  di,unsigned short  si,unsigned short  ds,unsigned short  es,unsigned short  dx,unsigned short  cx,unsigned short  bx,unsigned short  ax,unsigned short  ip,unsigned short  cs,unsigned short  flags){
        if (ax == 0) {
                es=FP_SEG(in_packet);
                di=FP_OFF(in_packet);
                in_packet_len=cx;
        }
        else {
                j=0;     
                for(i=6;i<12;i++)   cprintf("%02x:",in_packet[i]);
                cprintf("Received :");
                for(i=14;i<in_packet_len;i++){
                        if(in_packet[i]==0x00)    break;
                        putch(in_packet[i]);
                        num[j++] = in_packet[i];
                }
                n = factorial(atoi(num));
                sprintf(naya_buffer,"%lld",n);
                fill_packet(naya_buffer, strlen(naya_buffer));
                send_packet();                
                putch('\r');putch('\n');
                return;
        }
}
unsigned long long int factorial(unsigned int n){
        if(n==0 || n==1)        return 1;
        return n*factorial(n-1);
}
void fill_packet(unsigned char *m, int l){
        memcpy(packet+14,m,l);
        for(i=l+14;i<64;i++)
                packet[i] = 0;
}
void send_packet(){
        union REGS in,out;
        struct SREGS s;
        in.h.ah = 4;
        in.x.cx = 64;   
        in.x.si = FP_OFF(packet);
        s.ds = FP_SEG(packet);
        int86x(0x60,&in,&out,&s);
}
void getMac(){
        union REGS in,out;
        struct SREGS s;
        char far *buf;
        in.h.ah=6;
        in.x.cx=6;
        buf = (char far *)source_mac;
        in.x.di=FP_OFF(source_mac);
        s.es=FP_SEG(buf);
        int86x(0x60,&in,&out,&s);    
}
void fillHead(){
        memcpy(packet,dest_mac,6);
        memcpy(packet+6,source_mac,6);
        memcpy(packet+12,type,2);
}
void access_type(){
        union REGS in,out;
        struct SREGS s;
        in.h.ah = 2;
        in.h.al = 1;
        in.x.bx =-1;
        in.h.dl = 0;
        in.x.cx = 0;
        s.es = FP_SEG(receiver);
        in.x.di = FP_OFF(receiver);
        in.x.si = FP_OFF(type);
        s.ds = FP_SEG(type);
        int86x(0x60,&in,&out,&s);
        handle = out.x.ax;
}
void release_type(){
        union REGS in,out;
        struct SREGS s;
        in.h.ah=3;
        in.x.bx=handle;
        int86x(0x60,&in,&out,&s);
}
void main(){
        printf("Start :\n");
        getMac();
        fillHead();
        access_type();
        len_buf=0;        
        while(1){
                opt = getchar();
                if(opt==0x0A || opt==0x0D){
                        if(lit>=1)
                        break;
                        if(len_buf==0)
                        lit++;
                        fill_packet(buffer,len_buf,0);
                        len_buf=0;
                        send_packet();
                }
                else{
                        lit=0;
                        buffer[len_buf]=opt;
                        len_buf++;
                }       
        }
        release_type();
        printf("Exiting\n");     
        return;
}