import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/*
 * TODO: Make provisions for literal string of bytes?
 */
public class RLECompress {
	
	public static void main(String[] args) throws IOException {
		
		if(args.length!=2){
			System.out.println("Usage: <input file> <output file>");
			System.exit(10022);
		}
		
		File input = new File(args[0]), output = new File(args[1]);
		
		BufferedInputStream in = new BufferedInputStream(new FileInputStream(input));
		BufferedOutputStream out = new BufferedOutputStream(new FileOutputStream(output));
		
		int x,prevX=in.read(),len=1;
		
		while((x=in.read())!=-1){
			if(x==prevX)
				len++;
			else{
				while(len>0){
					out.write(Math.min(len, 255));
					out.write(prevX);
					len -= 255;
				}
				len = 1;
			}
			prevX = x;
		}
		
		while(len>0){
			out.write(Math.min(len, 255));
			out.write(prevX);
			len -= 255;
		}
		
		in.close();
		out.close();
		
		System.out.println(output.length());
		
	}
	
}
