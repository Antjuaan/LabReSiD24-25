import threading

# Funzione per la moltiplicazione
def multiply(data):
    data["result"] = data["a"] * data["b"]

# Funzione per la somma
def add(data):
    data["result"] = data["a"] + data["b"]

# Funzione per la sottrazione
def subtract(data):
    data["result"] = data["a"] - data["b"]


# Main
def main():
    # Inizializzazione dei dati
    data1 = {"a": 2, "b": 6, "result": 0}
    data2 = {"a": 1, "b": 4, "result": 0}
    data3 = {"a": 5, "b": 2, "result": 0}
    data4 = {"a": 0, "b": 0, "result": 0}
    data5 = {"a": 0, "b": 0, "result": 0}

    # Creazione e avvio dei primi tre thread
    thread1 = threading.Thread(target=multiply, args=(data1,))
    thread2 = threading.Thread(target=add, args=(data2,))
    thread3 = threading.Thread(target=subtract, args=(data3,)) 

    thread1.start()
    thread2.start()
    thread3.start()

    # Attesa del completamento dei primi tre thread
    thread1.join()
    thread2.join()
    thread3.join()

    data4["a"] = data2["result"]
    data4["b"] = data3["result"]
    data5["a"] = data1["result"]

    # Creazione e avvio del quarto thread
    thread4 = threading.Thread(target=multiply, args=(data4,))  
    thread4.start()

    # Attesa del completamento del quarto thread
    thread4.join()

    data5["b"] = data4["result"]

    # Creazione e avvio del quinto thread
    thread5 = threading.Thread(target=add, args=(data5,))
    thread5.start()

    # Attesa del completamento del quinto thread
    thread5.join()

    print("Risultato finale:", data5["result"])

if __name__ == "__main__":
    main()