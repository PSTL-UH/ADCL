package chartplot;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Image;

import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JPanel;

import utility.*;

@SuppressWarnings("serial")
public class WelcomeScreen extends JPanel
{
	Utility util = Utility.getInstance();
	int frameWidth = 0;
	int frameHeight = 0;
	JLabel backLabel;
	
	public WelcomeScreen(Dimension windowSize)
	{
		frameWidth = windowSize.width;
		frameHeight = windowSize.height;
		setLayout(null);
		setBackground(Color.WHITE);
		backLabel = addBackground();
		addCSLogo();
		addPSTLLogo();
		addADCLGUILogo();
	}
	
	private void addADCLGUILogo()
	{
		Image adcl = util.getImage("/icons/adcl_logo.png");
		JLabel adclLogo = new JLabel();
		ImageIcon adclIcon = new ImageIcon(adcl.getScaledInstance(160, 50, Image.SCALE_SMOOTH));
		adclLogo.setIcon(adclIcon);
		
		adclLogo.setBounds((frameWidth-adclIcon.getIconWidth())/2, (frameHeight)/2-adclIcon.getIconHeight(), adclIcon.getIconWidth(), adclIcon.getIconHeight());
		backLabel.add(adclLogo);		
	}

	private JLabel addBackground()
	{
		Image back = util.getImage("/icons/Background.png");
		JLabel backLogo = new JLabel();
		ImageIcon backIcon = new ImageIcon(back.getScaledInstance(frameWidth, frameHeight, Image.SCALE_SMOOTH));
		backLogo.setIcon(backIcon);
		
		backLogo.setBounds(0, 0, backIcon.getIconWidth(), backIcon.getIconHeight());
		add(backLogo);	
		return backLogo;
	}

	private void addPSTLLogo()
	{
		Image logoImage = util.getImage("/icons/logo.png");
		JLabel pstllogo = new JLabel();
		ImageIcon logoImageIcon = new ImageIcon(logoImage.getScaledInstance(200, 60, Image.SCALE_SMOOTH));
		pstllogo.setIcon(logoImageIcon);
		
		pstllogo.setBounds(0, frameHeight - 80, logoImageIcon.getIconWidth(), logoImageIcon.getIconHeight());
		backLabel.add(pstllogo);
	}

	private void addCSLogo() {
		Image cslogoImage = util.getImage("/icons/cslogo.png");
		JLabel cslogo = new JLabel();
		ImageIcon cslogoImageIcon = new ImageIcon(cslogoImage.getScaledInstance(110, 35, Image.SCALE_SMOOTH));
		cslogo.setIcon(cslogoImageIcon);
		cslogo.setBounds(frameWidth - 120, frameHeight - 70, cslogoImageIcon.getIconWidth(), cslogoImageIcon.getIconHeight());
		backLabel.add(cslogo);
	}

}
