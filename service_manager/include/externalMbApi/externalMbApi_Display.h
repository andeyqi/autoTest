# i f n d e f   _ _ E X T E R N A L _ M B A P I _ D I S P L A Y _ H _ _  
 # d e f i n e   _ _ E X T E R N A L _ M B A P I _ D I S P L A Y _ H _ _  
  
 # i n c l u d e   " c q _ s t d l i b / c q _ s t d l i b . h "  
 # i n c l u d e   " c q _ s t d l i b / c q s t l / v e c t o r . h "  
 # i n c l u d e   " s e r v i c e M a n a g e r S e r v i c e . h "  
  
 n a m e s p a c e   S e r v i c e M a n a g e r  
 {  
 	 e n u m   E N _ D I S P L A Y S V C E N G I N E _ I L L _ S T A T E  
 	 {  
 	 	 E N _ D I S P L A Y S V C E N G I N E _ I L L _ S T A T E _ O N ,  
 	 	 E N _ D I S P L A Y S V C E N G I N E _ I L L _ S T A T E _ O F F ,  
 	 	 E N _ D I S P L A Y S V C E N G I N E _ I L L _ S T A T E _ I N V A L I D ,  
 	 } ;  
 	 e n u m   D e S a i _ B a c k L i g h t M o d e  
 	 {  
 	 	 S M _ B L M _ M A N U A L   =   0 ,  
 	 	 S M _ B L M _ A U T O 	 / / 1  
 	 } ;  
 	 e n u m   D e S a i _ B a c k L i g h t T y p e  
 	 {  
 	 	 S M _ B L T _ D A Y   =   0 ,  
 	 	 S M _ B L T _ N I G H T 	 / / 1  
 	 } ;  
  
 	 e n u m   E N _ D I S P L A Y S V C E N G I N E _ W I D G E T _ S T A T E  
 	 {  
 	 	 E N _ D I S P L A Y S V C E N G I N E _ W I D G E T _ S H O W   =   0 ,  
 	 	 E N _ D I S P L A Y S V C E N G I N E _ W I D G E T _ H I D E ,  
 	 	 E N _ D I S P L A Y S V C E N G I N E _ W I D G E T _ I N V A L I D ,  
 	 } ;  
  
 	 t y p e d e f   s t r u c t   D e S a i _ B a c k L i g h t  
 	 {  
 	 	 D e S a i _ B a c k L i g h t M o d e   m o d e ;  
 	 	 D e S a i _ B a c k L i g h t T y p e   t y p e ;  
 	 	 i n t   l e v e l ;  
 	 } D e S a i _ B a c k L i g h t ;  
 	  
 	 / / H M I �e��<h_ 
 	 e n u m   E N _ D I S P L A Y S V C E N G I N E _ T I M E _ F O R M A T  
 	 {  
 	 	 E N _ D I S P L A Y S V C E N G I N E _ T I M E _ F O R M A T _ 1 2 H O U R   =   0 ,  
 	 	 E N _ D I S P L A Y S V C E N G I N E _ T I M E _ F O R M A T _ 2 4 H O U R ,  
 	 	 E N _ D I S P L A Y S V C E N G I N E _ T I M E _ F O R M A T _ U N K N O W N ,  
 	 } ;  
  
 	 t y p e d e f   s t r u c t   T i m e I n f o  
 	 {  
 	 	 u i n t 6 4   t i m e V a l u e ;  
 	 	 E N _ D I S P L A Y S V C E N G I N E _ T I M E _ F O R M A T   t i m e F o r m a t ;  
 	 } T i m e I n f o ;  
  
 	 c l a s s   D i s p l a y S v c E n g i n e L i s t e n e r  
 	 {  
 	 	 p u b l i c :  
 	 	 	 / / I L L �r`�r`�SS��w 
 	 	 	 v i r t u a l   v o i d   o n I L L S t a t e C h a n g e d N o t i f y ( E N _ D I S P L A Y S V C E N G I N E _ I L L _ S T A T E   d a t a )   {   U N U S E D _ V A R ( d a t a ) ;   }  
 	 	 	 / / ̀IQ�r`�SS��w 
 	 	 	 v i r t u a l   v o i d   o n B a c k l i g h t M o d e C h a n g N o t i f y ( D e S a i _ B a c k L i g h t *   d a t a )   {   U N U S E D _ V A R ( d a t a ) ;   }  
 	 	 	 v i r t u a l   v o i d   o n L a n g u a g e I d C h a n g N o t i f y ( i n t 3 2 *   d a t a )   {   U N U S E D _ V A R ( d a t a ) ;   }  
 / / 	 	 	 v i r t u a l   v o i d   o n L a n g u a g e S w i t c h N o t i f y ( u i n t 3 2 *   d a t a )   {   U N U S E D _ V A R ( d a t a ) ;   }  
 	 	 	 / /   ��BlM u s i c W i d g e t >f:y�V���w 
 	 	 	 v i r t u a l   v o i d   o n M u s i c W i d g e t S h o w N o t i f y ( b o o l   d a t a )   {   U N U S E D _ V A R ( d a t a ) ;   }  
 	 	 	 / /   ��BlM u s i c W i d g e t ��υ�V���w 
 	 	 	 v i r t u a l   v o i d   o n M u s i c W i d g e t H i d e N o t i f y ( b o o l   d a t a )   {   U N U S E D _ V A R ( d a t a ) ;   }  
 	 	 	 / /   ��BlM u s i c W i d g e t ��υ�V���w 
 	 	 	 v i r t u a l   v o i d   o n M u s i c W i d g e t I s S h o w N o t i f y ( b o o l   d a t a )   {   U N U S E D _ V A R ( d a t a ) ;   }  
 	 	 	 / /   f�:g�|�~�e���SS��w 
 	 	 	 v i r t u a l   v o i d   o n S y s H m i T i m e C h a n g e N o t i f y ( u i n t 6 4   d a t a )   {   U N U S E D _ V A R ( d a t a ) ;   }  
 	 	 	 / /   �xd�N�N�O�SO��NsQ���w 
 	 	 	 v i r t u a l   v o i d   o n T o u c h S t a t u s C h a n g e N o t i f y ( b o o l   d a t a ) {   U N U S E D _ V A R ( d a t a ) ;   }  
 	 	 	 / /   �e��<h_�r`��w 
 	 	 	 v i r t u a l   v o i d   o n T i m e F o r m a t N o t i f y ( E N _ D I S P L A Y S V C E N G I N E _ T I M E _ F O R M A T   d a t a ) {   U N U S E D _ V A R ( d a t a ) ;   }  
 	 } ;  
 	  
 	 c l a s s   D i s p l a y S v c E n g i n e   :   p u b l i c   S e r v i c e M a n a g e r S e r v i c e   <   D i s p l a y S v c E n g i n e L i s t e n e r   >  
 	 {  
 	 	 p u b l i c :  
 	 	 	 s t a t i c   D i s p l a y S v c E n g i n e *   g e t I n s t a n c e ( ) ;  
 	 	 	 s t a t i c   v o i d   r e l e a s e I n s t a n c e ( ) ;  
  
 	 	 	 b o o l   I n i t I n s t a n c e ( ) ;  
 	 	 	 b o o l   E x i t I n s t a n c e ( ) ;  
  
 	 	 	 E N _ D I S P L A Y S V C E N G I N E _ I L L _ S T A T E   R e q C h e c k I L L S t a t e ( ) ;  
  
 # i f   0  
 	 	 	 B O O L   D i s p l a y S v c E n g i n e _ R e q S h o w O t h e r F o r m ( ) ;  
 	 	 	 B O O L   D i s p l a y S v c E n g i n e _ R e q H i d e O t h e r F o r m ( ) ;  
 # e n d i f  
 / / 	 	 	 v o i d   D i s p l a y S v c E n g i n e _ R e t L a n g u a g e S w i t c h ( b o o l   d a t a ) ;  
 	 	 	 / / MRT�SRbc 
 	 	 	 v o i d   R e q B a c k g r o u n d S w i t c h ( v o i d ) ;  
  
 	 	 	 v o i d   G e t B a c k l i g h t M o d e ( ) ;  
 	 	 	 i n t 3 2   G e t L a n g u a g e I D ( v o i d ) ;  
  
 	 	 	 v o i d   D i s p l a y S v c E n g i n e _ B a c k T o H o s t ( v o i d ) ;  
 	 	 	 v o i d   D i s p l a y S v c E n g i n e _ S h o w H o s t M e n u ( v o i d ) ;  
  
 	 	 	 / /   ��Bl>f:yM u s i c W i d g e t  
 	 	 	 v o i d   D i s p l a y S v c E n g i n e _ R e q S h o w M u s i c W i d g e t ( ) ;  
 	 	 	 / /   ��Bl��υM u s i c W i d g e t  
 	 	 	 v o i d   D i s p l a y S v c E n g i n e _ R e q H i d e M u s i c W i d g e t ( ) ;  
 	 	 	 / /   ���SM u s i c W i d g e t >f:y�r` 
 	 	 	 E N _ D I S P L A Y S V C E N G I N E _ W I D G E T _ S T A T E   D i s p l a y S v c E n g i n e _ G e t M u s i c W i d g e t S t a t u s ( ) ;  
 	 	 	 / /   ��wf�:g�S�NۏL�MRT�SRbc 
 	 	 	 v o i d   D i s p l a y S v c E n g i n e _ R e a d y F o r F o r g r o u n d ( v o i d ) ;  
 	 	 	 / /   ���S�e��<h_ 
 	 	 	 E N _ D I S P L A Y S V C E N G I N E _ T I M E _ F O R M A T   g e t T i m e F o r m a t ( ) ;  
  
 	 	 p r i v a t e :  
 	 	 	 D i s p l a y S v c E n g i n e ( ) ;  
 	 	 	 ~ D i s p l a y S v c E n g i n e ( ) ;  
 	 	 	 D i s p l a y S v c E n g i n e ( D i s p l a y S v c E n g i n e &   d i s p l a y ) ;  
 	 	 	 D i s p l a y S v c E n g i n e &   o p e r a t o r = ( D i s p l a y S v c E n g i n e &   d i s p l a y ) ;  
  
  
 	 	 	 v o i d   i l l S t a t e C h a n g e d N o t i f y ( E N _ D I S P L A Y S V C E N G I N E _ I L L _ S T A T E   d a t a ) ;  
 	 	 	 v o i d   b a c k l i g h t M o d e C h a n g N o t i f y ( D e S a i _ B a c k L i g h t *   d a t a ) ;  
 	 	 	 v o i d   l a n g u a g e I d C h a n g N o t i f y ( i n t 3 2 *   d a t a ) ;  
 / / 	 	 	 v o i d   D i s p l a y S v c E n g i n e _ L a n g u a g e S w i t c h N o t i f y ( v o i d   * d a t a ) ;  
  
 	 	 	 f r i e n d   v o i d   i l l S t a t e C h a n g e d N o t i f y ( v o i d *   p a r a m ) ;  
 / / 	 	 	 f r i e n d   v o i d   L a n g u a g e S w i t c h N o t i f y ( v o i d   * d a t a )  
 # i f   0  
 	 	 	 f r i e n d   v o i d   D i s p l a y S v c E n g i n e _ S g n S h o w O t h e r F o r m S u c ( v o i d *   d a t a ) ;  
 	 	 	 f r i e n d   v o i d   D i s p l a y S v c E n g i n e _ S g n H i d e O t h e r F o r m S u c ( v o i d *   d a t a ) ;  
 # e n d i f  
 	 	 	 f r i e n d   v o i d   D i s p l a y S v c E n g i n e _ S g n I L L S t a t e ( v o i d *   d a t a ) ;  
 	 	 	 f r i e n d   v o i d   D i s p l a y S v c E n g i n e _ A n s w e r B a c k l i g h t M o d e ( v o i d *   d a t a ) ;  
 	 	 	 f r i e n d   v o i d   D i s p l a y S v c E n g i n e _ B a c k l i g h t M o d e C h a n g e N o t i f y ( v o i d *   p D a t a ) ;  
 	 	 	 f r i e n d   v o i d   D i s p l a y S v c E n g i n e _ A n s w e r L a n g u a g e I D ( v o i d *   d a t a ) ;  
 	 	 	 f r i e n d   v o i d   D i s p l a y S v c E n g i n e _ L a n g u a g e I D C h a n g e N o t i f y ( v o i d *   d a t a ) ;  
 / / 	 	 	 f r i e n d   v o i d   D i s p l a y S v c E n g i n e _ L a n g u a g e S w i t c h C h a n g e d ( v o i d *   d a t a ) ;  
  
 	 	 p r i v a t e :  
 	 	 	 s t a t i c   D i s p l a y S v c E n g i n e *   m _ i n s t a n c e ;  
  
 	 	 	 i n t 3 2   m _ l a n g u a g e I d ;  
 	 	 	 u i n t 3 2   m _ m o d e ;  
 	 	 	 u i n t 3 2   m _ t y p e ;  
 	 	 	 E N _ D I S P L A Y S V C E N G I N E _ I L L _ S T A T E   m _ i l l S t a t u s ;  
  
 	 	 	 H a n d l e   m _ d i s p l a y S v c S h o w O t h e r E v e n t ;  
 	 	 	 H a n d l e   m _ d i s p l a y S v c H i d e O t h e r E v e n t ;  
 	 	 	 H a n d l e   m _ d i s p l a y S v c E v e n t I L L ;  
 	 	 	 H a n d l e   m _ d i s p l a y S v c B a c k L i g h t E v e n t ;  
 	 	 	 H a n d l e   m _ d i s p l a y S v c L a n g u a g e I d E v e n t ;  
 	 } ;  
 }  
  
 # e n d i f   / *   _ _ E X T E R N A L _ M B A P I _ D I S P L A Y _ H _ _   * /  
 