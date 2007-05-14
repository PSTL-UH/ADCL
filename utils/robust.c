/* applying a t-model to performance data to obtain a robust
   estimate of the mean via ML of the loglikelihood function */
/*gcc robust.c -I/usr/local/include/gsl/ -L/usr/local/lib -lgsl -L/opt/intel/mkl/9.0/lib/32/ -lmkl_ia32 -lguide -o robust*/

#include <math.h>
#include <gsl_math.h>
#include <gsl_sf_gamma.h>
#include <gsl_sf_log.h>
#include <gsl_multimin.h>
#include <gsl_statistics_double.h>


double loglikelihood(const gsl_vector *x, void *params){
   double mu, sigma2;
   int n;
   double nu, *data;
   double y;
   int i;
   double * tmp = (double *) params, delta, g;

   /* extract arguments and parameters */
   mu     = gsl_vector_get(x, 0);
   sigma2 = gsl_vector_get(x, 1);
   nu     = tmp[0];
   n      = (int) tmp[1];
   data   = &(tmp[2]);
   //printf("%d %f %f\n", n, nu, data[0]);

   if (nu == 0.0){ 
      /* optimize for nu, compute whole log likelyhood function */
      nu = gsl_vector_get(x,2);
      g =  gsl_sf_lngamma((nu+1)/2) - gsl_sf_lngamma(nu/2)
           - 0.5 * gsl_sf_log(M_PI*nu);
   }
   else { /* life's simpler */
      g = 0.0;
   }

   y = 0.;
   for (i=0; i<n; i++){
     delta = (data[i] - mu)*( data[i] - mu) / (nu * sigma2);
     if (sigma2 > 0.0 || delta > 0.0){ 
        y = y - 0.5 * gsl_sf_log(sigma2) 
              - 0.5 * (nu + 1) * gsl_sf_log(1+delta) + g;
     }
     else { /* dirty */
        y = y + 100;
        printf("Not good. Modify step size and tolerance\n");
     }
   }

   //printf("mu=%f.5, sigma2=%f.5, y=%f.5\n", mu, sigma2, y);
   return -y; /* since we're minimizing, not maximizing */

}

int  minimize_loglikelihood(gsl_multimin_fminimizer *s,
   const int maxiter, double* nu, double* mu, double *sigma, double* val){
   /* 
   s       (in)      GSL multimin fminimizer object
   maxiter (in)      maximum number of iterations
   nu      (in/out)  if nu is 0, optimize also for nu and return 
                     optimized value
                     if nu is != 0, accept nu as parameter
   mu      (out)     optimized value for mu
   sigma   (out)     optimized value for sigma
   val     (out)     if nu is variable, value of log likelihood function 
                     if nu is fixed, value of simplified log likelihood
                     function */
   size_t iter = 0;
   int status;
   double size;

   do
   {
      status = gsl_multimin_fminimizer_iterate(s);
      if (status) break;

      size   = gsl_multimin_fminimizer_size(s);
      status = gsl_multimin_test_size(size, 1e-4); /* ???? */

      if (status == GSL_SUCCESS)
         printf("Maximum found after %d iterations at:\n", iter);

      /*printf("%5d %.5f %.5f", iter, 
         gsl_vector_get(s->x, 0),
         sqrt(gsl_vector_get(s->x, 1)));
      if (*nu == 0.0) printf("%.5f", gsl_vector_get(s->x, 2));
      printf("%10.5f\n", -gsl_multimin_fminimizer_minimum(s));*/
      iter++;

   } while (status == GSL_CONTINUE && iter < maxiter);

   *mu    = gsl_vector_get(s->x, 0);
   *sigma = sqrt(gsl_vector_get(s->x, 1));
   if (*nu == 0.0)  /* return optimized value */ 
      *nu = gsl_vector_get(s->x, 2);
   *val   = -gsl_multimin_fminimizer_minimum(s);
   printf("%.5f %.5f %.5f %.5f\n", *nu, *mu, *sigma, *val);

   return 0;
}

int ml(const int ndata,  double* const data, 
       double* nu, double *mu, double *sigma, double *val){
/* performs a ML method for a t model
   ndata  (in)      size of data
   data   (in)      data / measurements
   nu     (in/out)  if nu is 0, optimize also for nu and return 
                    optimized value
                    if nu is != 0, accept nu as parameter
   mu     (out)     optimized value for mu
   sigma  (out)     optimized value for sigma
   val    (out)     if nu is variable, value of log likelihood function 
                    if nu is fixed, value of simplified log likelihood
                    function*/
   const gsl_multimin_fminimizer_type *T;
   gsl_multimin_fminimizer *s;
   int i, maxiter=200;
   gsl_vector *x0;                   /* starting point */
   gsl_vector *stepsize;
   gsl_multimin_function my_func;
   double mean, variance, median, nustart=4.0;
   double *params;

   /* init starting point */
   if (*nu != 0.0)   x0 = gsl_vector_alloc(2);
   else              x0 = gsl_vector_alloc(3);
   mean = gsl_stats_mean(data, (size_t) 1,(size_t) ndata);
   /*gsl_sort(data, 1, ndata);
   median = gsl_stats_median_from_sorted_data(data, (size_t) 1, (size_t) ndata);
   mean = median; */
   variance = gsl_stats_variance_with_fixed_mean(data, 
      (size_t) 1, (size_t) ndata, mean);
   gsl_vector_set(x0, 0, mean);
   gsl_vector_set(x0, 1, variance);
   if (*nu == 0.0)   gsl_vector_set(x0, 2, nustart);

   /* init parameters */
   params = malloc((ndata+2)*sizeof(double));
   params[0] = *nu;
   params[1] = (double) ndata;
   for (i=0; i<ndata; i++){
      params[2+i] = data[i];
   }

   /* set stepsize */
   if (*nu != 0.0)   stepsize = gsl_vector_alloc(2);
   else              stepsize = gsl_vector_alloc(3);
   gsl_vector_set(stepsize, 0, 0.001);
   gsl_vector_set(stepsize, 1, 0.001);
   if (*nu == 0.0)   gsl_vector_set(stepsize, 2, 0.0001);

   /* set up solver */
   T = gsl_multimin_fminimizer_nmsimplex;
   if (*nu != 0.0)  s = gsl_multimin_fminimizer_alloc(T,2);
   else             s = gsl_multimin_fminimizer_alloc(T,3);

   /* init function object */
   my_func.f = &loglikelihood;
   if (*nu != 0.0)   my_func.n = 2;
   else              my_func.n = 3;
   my_func.params = params;

   /* call minimizer */
   gsl_multimin_fminimizer_set(s, &my_func, x0, stepsize);
   minimize_loglikelihood(s, maxiter, nu, mu, sigma, val);

   /* clean up */
   gsl_multimin_fminimizer_free(s);
   gsl_vector_free(x0);
   free (params);

   return 0;
}


#ifdef OWN_MAIN

int main(){
   /* speed of light data set
      for variable nu:
         mu_hat = 27.40, sigma_hat = 3.81, vu = 2.13
      for fixed nu=4:
         muhat = 27.49, sigma = 4.51 */
   const int ndata=66;
   double data[66] = { 
      28.0, 26.0, 33.0, 24.0, 34.0, -44.0, 27.0, 16.0, 40.0, -2.0,
      29.0, 22.0, 24.0, 21.0, 25.0, 30.0, 23.0, 29.0, 31.0, 19.0,
      24.0, 20.0, 36.0, 32.0, 36.0, 28.0, 25.0, 21.0, 28.0, 29.0,
      37.0, 25.0, 28.0, 26.0, 30.0, 32.0, 36.0, 26.0, 30.0, 22.0,
      36.0, 23.0, 27.0, 27.0, 28.0, 27.0, 31.0, 27.0, 26.0, 33.0,
      26.0, 32.0, 32.0, 24.0, 39.0, 28.0, 24.0, 25.0, 32.0, 25.0,
      29.0, 27.0, 28.0, 29.0, 16.0, 23.0};
   int i, varnu=0;
   double nu, mu, sigma, val;

   nu = 0.0; 
   //nu =  4.0;
   ml(ndata, data, &(nu), &(mu), &(sigma), &(val));

   //printf("%.5f %.5f %.5f %f\n", nu, mu, sigma, val);

   return 0;
}
#endif

