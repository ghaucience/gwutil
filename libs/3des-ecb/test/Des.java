/**
 * Created by Elskov on 2017-08-02.
 */

import java.security.Key;
import java.security.MessageDigest;
import javax.crypto.SecretKey;
import javax.crypto.Cipher;
import javax.crypto.spec.SecretKeySpec;

import com.sun.org.apache.xerces.internal.impl.dv.util.Base64;
//import com.sun.org.apache.xml.internal.security.algorithms.JCEMapper;

@SuppressWarnings("restriction")
public class Des {

    private static final String Algorithm = "DESede"; 

    public static void main(String[] args) throws Exception {

        System.out.println("1");

				/*
        byte[] key = {0x11, 0x22, 0x4F, 0x58,
                (byte)0x88, 0x10, 0x40, 0x38, 0x28, 0x25, 0x79, 0x51,
                (byte)0xCB,
                (byte)0xDD, 0x55, 0x66, 0x77, 0x29, 0x74,
                (byte)0x98, 0x30, 0x40, 0x36,
                (byte)0xE2
        };
				*/

        byte[] key = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x31, 0x32, 0x33,
										0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x31, 0x32, 0x33};





        String password = "dongsheng";
        String fin = Des.Encrypt3DES(password, key);
        System.out.println(fin);

        System.out.println("1");
    }

    //public static String Encrypt3DES(String value, String key) throws Exception {
    public static String Encrypt3DES(String value, byte[] key) throws Exception {
        return byte2Base64(encryptMode(key, value.getBytes()));
    }

    public static byte[] encryptMode(byte[] keybyte, byte[] src) {
        try {
            SecretKey deskey = new SecretKeySpec(keybyte, Algorithm); 
            Cipher c1 = Cipher.getInstance(Algorithm);
            c1.init(Cipher.ENCRYPT_MODE, deskey);
            return c1.doFinal(src);
        } catch (java.security.NoSuchAlgorithmException e1) {
            e1.printStackTrace();
        } catch (javax.crypto.NoSuchPaddingException e2) {
            e2.printStackTrace();
        } catch (java.lang.Exception e3) {
            e3.printStackTrace();
        }
        return null;
    }

    public static byte[] GetKeyBytes(String strKey) throws Exception {
        if (null == strKey || strKey.length() < 1)
            throw new Exception("key is null or empty!");
        MessageDigest alg = MessageDigest.getInstance("MD5");
        alg.update(strKey.getBytes());
        byte[] bkey = alg.digest();
        int start = bkey.length;
        byte[] bkey24 = new byte[24];
        for (int i = 0; i < start; i++) {
            bkey24[i] = bkey[i];
        }
        for (int i = start; i < 24; i++) {
            bkey24[i] = bkey[i - start];
        }
        return bkey24;
    }

    public static String byte2Base64(byte[] b) {
        return Base64.encode(b);
    }
}
