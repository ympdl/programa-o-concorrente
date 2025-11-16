//Atividade 3

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import java.util.ArrayList;
import java.util.List;

//classe runnable
class MyCallable implements Callable<Long> {
  //construtor
    public MyCallable() {}

  //método para execução
  public Long call() throws Exception {
    long s = 0;
    for (long i = 1; i <= 100; i++) {
      s++;
    }
    return s;
  }
}

//classe runnable para teste de primalidade
class PrimoCallable implements Callable<Boolean> {

  private final int n;

  public PrimoCallable(int n) {
    this.n = n;
  }

  // Verifica se um número é primo
  private boolean ehPrimo(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    int limite = (int) Math.sqrt(n) + 1;
    for (int i = 3; i < limite; i += 2) {
      if (n % i == 0) return false;
    }
    return true;
  }

  // Retorna TRUE se for primo, FALSE caso contrário
  public Boolean call() {
    return ehPrimo(n);
  }
}

//classe do método main
public class FutureHello3  {

  private static final int NTHREADS = 10;

  public static void main(String[] args) {

    // -------- NOVO: Pega N pelos argumentos do terminal --------
    if (args.length == 0) {
      System.out.println("Uso: java FutureHello3 <N>");
      return;
    }
    int NPRIMOS = Integer.parseInt(args[0]);
    // -----------------------------------------------------------

    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
    List<Future<Boolean>> resultadosPrimos = new ArrayList<>();

    for (int i = 1; i <= NPRIMOS; i++) {
      Callable<Boolean> worker = new PrimoCallable(i);
      Future<Boolean> submit = executor.submit(worker);
      resultadosPrimos.add(submit);
    }

    int totalPrimos = 0;
    for (Future<Boolean> f : resultadosPrimos) {
      try {
        if (f.get()) totalPrimos++;
      } catch (Exception e) { e.printStackTrace(); }
    }

    System.out.println("Quantidade de números primos entre 1 e " + NPRIMOS + ": " + totalPrimos);

    executor.shutdown();
  }
}