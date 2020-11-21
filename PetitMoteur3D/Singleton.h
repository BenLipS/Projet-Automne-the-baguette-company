#pragma once

namespace PM3D
{
    //
	//   TEMPLATE�: CSingleton
	//
	//   BUT�: Template servant � construire un objet unique
	//
	//   COMMENTAIRES�:
	//
	//        Comme plusieurs de nos objets repr�senteront des �l�ments uniques 
	//		  du syst�me (ex: le moteur lui-m�me, le lien vers 
	//        le dispositif Direct3D), l'utilisation d'un singleton 
	//        nous simplifiera plusieurs aspects.
	//

	template <class T> class CSingleton
	{
	public:
        // Renvoie l'instance unique de la classe
        static T& GetInstance() noexcept
        {
            return Instance;
        }

	protected :

        // Constructeur par d�faut
		CSingleton(void) noexcept{}
	
        // Destructeur
		~CSingleton(void){}

	private :

        // Donn�es membres
        static T Instance; // Instance de la classe

		// D�clarer les op�rations de copie sans les
		// d�finir, pour emp�cher leur utilisation
        CSingleton(CSingleton&);
        void operator =(CSingleton&);
  
	};

    // Instanciation du singleton
    template <class T> T CSingleton<T>::Instance ;
}