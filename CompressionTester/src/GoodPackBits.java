import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * My implementation of the PackBits RLE format.
 * See https://wiki.nesdev.com/w/index.php/Tile_compression
 * It has provisions for runs and literal groups of bytes.
 * 
 * The PackBits format is as follows:
 * 00-7F: Write the following n+1 bytes to output
 * 80:    NOP (ignore this byte)
 * 81-FF: Write the following byte to output 257-n times
 * 
 * But why is there a NOP? What purpose does it serve?
 * Well, it limits run lengths and literal lengths to 128, which is a nice power of two.
 * If you have a run of more than max size, you have to convert it into multiple
 * smaller runs of max size. This can be done with division and mod. But if the max
 * length is 128, this can be done with (n>>7) and (n & 0x7F), which would be especially 
 * useful on old computers where division was prohibitively slow.
 * 
 * But it's 2019! My computer can handle division just fine, and yours can too!
 * So, I've created a simple modification to this format:
 * 
 * NOPless PackBits:
 * 00-7F: Write the following n+1 bytes to output
 * 80-FF: Write the following byte to output 257-n times
 * 
 * I made 80 count as a valid run length. That's it. Runs can be up to 129 bytes long.
 * 
 * To switch between NOPless and true PackBits, run this program with or without the
 * "-nopless" flag before the input/output folders.
 * NOPless only makes a meaningful difference for truly massive runs, so it's probably
 * best to stick to true PackBits most of the time.
 * 
 * @author Ryan
 *
 */
public class GoodPackBits{
	
	//Max run length to be used.
	//Setting this to 128 results in the PackBits spec.
	//Play around with other numbers for fun! However, they won't be
	//encoded in the file.
	public static final int MAX_RUN_LEN = 128;
	
	//Real packbits has a NOP value, for some reason...
	//set this flag to true to create this behavior.
	//Set to false to allow 0x80 to be used as a NOP.
	public static boolean TRUE_PACKBITS = true;
	
	public static void main(String[] args) throws IOException {
		
		int i;
		for(i=0;i<args.length;i++){
			if(args[i].charAt(0)!='-')
				break;
			if(args[i].equals("-nopless")){
				System.out.println("Running nopless PackBits...");
				TRUE_PACKBITS = false;
			}
		}
		
		if(TRUE_PACKBITS){
			System.out.println("Running true PackBits...");
		}
		
		if(args.length-i!=2){
			System.out.println("Usage: [-nopless] <input file> <output file>");
			System.exit(10022);
		}
		
		File input = new File(args[i]), output = new File(args[i+1]);
		
		BufferedInputStream in = new BufferedInputStream(new FileInputStream(input));
		BufferedOutputStream out = new BufferedOutputStream(new FileOutputStream(output));
		
		int x,prevX=in.read(),len=1,buffSize=0;
		byte buff[] = new byte[257-MAX_RUN_LEN];
		
		while((x=in.read())!=-1){
			if(x==prevX)
				len++;
			else{
				if(len==1){
					buff[buffSize++] = (byte)prevX;
				}else{
					if(buffSize>0){
						out.write(buffSize-1);
						out.write(buff,0,buffSize);
						buffSize = 0;
					}
					while(len>0){
						//group big runs into maximally sized smaller ones.
						//True packbits leaves room for a NOP value.
						//My good version adds 1 to the max run length and uses the NOP value
						//to encode run length.
						//It's the same number of iterations as division/modulus.
						int max = TRUE_PACKBITS ? MAX_RUN_LEN : MAX_RUN_LEN+1;
						int runLen = Math.min(len, max);
						out.write(257-runLen);
						out.write(prevX);
						len -= max;
					}
				}
					
				len = 1;
				
				if(buffSize==buff.length){
					out.write(buffSize-1);
					out.write(buff);
					buffSize = 0;
				}
			}
			prevX = x;
		}
		
		if(len>1){
			while(len>0){
				int max = TRUE_PACKBITS ? MAX_RUN_LEN : MAX_RUN_LEN+1;
				int runLen = Math.min(len, max);
				out.write(257-runLen);
				out.write(prevX);
				len -= max;
			}
		}else{
			buff[buffSize++] = (byte) prevX;
			out.write(buffSize-1);
			out.write(buff,0,buffSize);
		}
		
		in.close();
		out.close();
		
		/*
		 * int upper = len>>7, lower = len & 0x7F;
		 * while(upper-->0){
		 *     out.write(0x81);
		 *     out.write(prevX);
		 * }
		 * out.write(lower);
		 * out.write(prevX);
		 * 
		 */
		
		System.out.println(output.length());
		
	}
	
}
