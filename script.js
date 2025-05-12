 <script>
        $(document).ready(function() {
            // Configuração da DataTable
            $('#tabela').DataTable({
                "language": {
                    "url": "//cdn.datatables.net/plug-ins/1.13.4/i18n/pt-BR.json"
                },
                "pageLength": 10,
                "responsive": true
            });

            // Configuração do Gráfico
            const ctx = document.getElementById('grafico').getContext('2d');
            const grafico = new Chart(ctx, {
                type: 'pie',
                data: {
                    labels: ['Movimento Detectado', 'Sem Movimento'],
                    datasets: [{
                        label: 'Presenças',
                        data: [{{ detectado }}, {{ sem }}],
                        backgroundColor: ['#e74c3c', '#2ecc71'],
                        borderWidth: 1
                    }]
                },
                options: {
                    responsive: true,
                    plugins: {
                        legend: {
                            position: 'bottom',
                            labels: {
                                color: '#ffffff',
                                font: {
                                    family: 'JetBrains Mono'
                                }
                            }
                        }
                    }
                }
            });
        });
    </script>